#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "glew-2.1.0/include/GL/glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui-1.65/imgui_impl_sdl.h"
#include "imgui-1.65/imgui_impl_opengl2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include <vector>

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#pragma comment (lib, "glew-2.1.0/libx86/glew32.lib")

#pragma comment (lib, "Assimp/libx86/assimp.lib")	

void myCallback(const char *msg, char *userData) { LOG("%s",msg); }

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		LOG("Glew could not init %s\n", glewGetErrorString(error));
	}
	else
		LOG("Using Glew %s", glewGetString(GLEW_VERSION));

	if(ret == true)
	{
		//Use Vsync
		if(SDL_GL_SetSwapInterval(0) < 0)
			LOGW("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		SetLightAmbient();
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
	//	lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_TEXTURE_2D);
//		glShadeModel(GL_SMOOTH);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	ImGui::StyleColorsDark();

	return ret;
}

bool ModuleRenderer3D::Start()
{
	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	//future Assimp Logs in console
	struct aiLogStream stream;
	stream.callback = myCallback;
	aiAttachLogStream(&stream);
	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return true;
}


// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();

	if (App->time->gameState == GameState_NONE)
		currentCam = App->camera->SetEditorCamera();
	else if (App->time->gameState != GameState_TICK)
	{
		currentCam = App->geometry->GetPlayingCamera();
		currentCam->UpdateFrustum();
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(currentCam->GetViewMatrix().ptr());

	// Light 0 on cam pos
	lights[0].SetPos(App->camera->GetPos());

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate()
{

	// 1. Draw geometry Camera Culling
	std::vector<GameObject*> drawerGO;
	if (cameraCulling)//Only draw camera View
	{
		App->sceneIntro->octree.GetIntersects(drawerGO, App->geometry->GetPlayingCamera()->frustum);

		for (auto iterator : drawerGO)
			DrawOctreeObjects(iterator);
	}
	else
	{//Draw all
		App->sceneIntro->octree.GetGameObjects(drawerGO);
		for (auto iterator : drawerGO)
			DrawOctreeObjects(iterator);
	}

	//Draw Dynamic objects (there aren't in octree)
	if(!App->gameObject->dynamicObjects.empty())
		DrawDynamicObjects(cameraCulling);

	// 2. Debug geometry
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	if (debugQuad)
		DebugDraw();

	else
	{
		if (App->geometry->currentGameObject != nullptr)
		{
			static float3 corners[8];
			App->geometry->currentGameObject->transform->boundingBox.GetCornerPoints(corners);

			DrawQuad(corners, Green);

			Camera* cam = App->geometry->GetPlayingCamera();
			cam->UpdateFrustum();
			if((Camera*)App->geometry->currentGameObject->GetComponent(ComponentType_CAMERA) == cam)
			{
				static float3 corners[8];
				cam->frustum.GetCornerPoints(corners);

				DrawQuad(corners, Green);
			}

		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);


	// 3. Draw UI
	App->gui->DrawUI();

	// 4. Swap buffers
	SDL_GL_SwapWindow(App->window->window);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	auto Width = DM.w;
	auto Height = DM.h;

	return UPDATE_CONTINUE;
}
void ModuleRenderer3D::DrawOctreeObjects(GameObject * iterator)
{
	if (iterator->GetActive())
	{
		Geometry* currentGeometry = (Geometry*)(iterator)->GetComponent(ComponentType_GEOMETRY);
		if (currentGeometry)
			DrawObject(currentGeometry);
	}
}
void ModuleRenderer3D::DrawDynamicObjects(bool cameraCulling)
{
	for (std::list<GameObject*>::iterator it = App->gameObject->dynamicObjects.begin(); it != App->gameObject->dynamicObjects.end(); ++it)
	{
		Geometry* currentGeometry = (Geometry*)(*it)->GetComponent(ComponentType_GEOMETRY);
		if (currentGeometry && (*it)->GetActive())
		{
			if (!cameraCulling || currentCam->frustum.Intersects(*(*it)->GetAABB()))
				DrawObject(currentGeometry);
		}
	}
}
void ModuleRenderer3D::DrawObject(Component * geometry)
{
	switch (((Geometry*)geometry)->GetType())
	{
	case Primitive_Point:
		((Geometry*)geometry)->Render();
		break;
	case Primitive_Ray:
		((RayLine*)geometry)->Render();
		break;
	case Primitive_Plane:
		((PrimitivePlane*)geometry)->Render();
		break;
	case Primitive_Cube:
		((PrimitiveCube*)geometry)->Render();
		break;
	case Primitive_Sphere:
		((PrimitiveSphere*)geometry)->Render();
		break;
	case Primitive_Cylinder:
		((PrimitiveCylinder*)geometry)->Render();
		break;
	case Primitive_Frustum:
		((PrimitivePlane*)geometry)->Render();
		break;
	case Primitive_Mesh:
		((Mesh*)geometry)->Render();
		break;
	default:
		break;
	}
}
// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	aiDetachAllLogStreams();
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ProjectionMatrix = Perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf((GLfloat*)ProjectionMatrix.ptr());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	int w, h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	currSize.x = w;
	currSize.y = h;

	float2 size;
	size.x = currSize.x / prevSize.x;
	size.y = currSize.y / prevSize.y;

	//App->gui->ResizeImGui(size);

	prevSize = currSize;
}

void ModuleRenderer3D::SetLightAmbient()
{
	GLfloat LightModelAmbient[] = { ambient_lihgt.x, ambient_lihgt.y, ambient_lihgt.z, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
}

math::float4x4 ModuleRenderer3D::Perspective(float fovy, float aspect, float n, float f) const
{
	math::float4x4 Perspective;
	float coty = 1.0f / tan(fovy * (float)PI / 360.0f);
	float cotA = coty / aspect;
	float nfnf = (n + f) / (n - f);
	float nf2 = 2.0f * n * f / (n - f);

	Perspective[0][0] = cotA;	Perspective[0][1] = 0.0f;	Perspective[0][2] = 0.0f;	Perspective[0][3] = 0.0f;
	Perspective[1][0] = 0.0f; 	Perspective[1][1] = coty;	Perspective[1][2] = 0.0f;	Perspective[1][3] = 0.0f;
	Perspective[2][0] = 0.0f; 	Perspective[2][1] = 0.0f;	Perspective[2][2] = nfnf;	Perspective[2][3] = -1.0f;
	Perspective[3][0] = 0.0f;	Perspective[3][1] = 0.0f;	Perspective[3][2] = nf2; 	Perspective[3][3] = 0.0f;
	

	return Perspective;
}


void ModuleRenderer3D::DebugDraw()
{
	std::vector<const OctreeNode*> aabb;
	App->sceneIntro->octree.GetBoxLimits(aabb);
	//Octree draw
	for (std::vector<const OctreeNode*>::const_iterator iterator = aabb.begin(); iterator != aabb.end(); ++iterator)
	{
		static float3 corners[8];
		(*iterator)->limits.GetCornerPoints(corners);

		DrawQuad(corners, Orange);
	}

	//Frustum draw
	/*static float3 corners[8];
	App->camera->GetCameraFrustrum().GetCornerPoints(corners);
	DrawQuad(corners, Blue);*/

	std::vector<GameObject*> objects;
	App->sceneIntro->octree.GetGameObjects(objects);
	//Octree Objects
	for (std::vector<GameObject*>::const_iterator iterator = objects.begin(); iterator != objects.end(); ++iterator)
	{
		static float3 corners[8];
		(*iterator)->transform->boundingBox.GetCornerPoints(corners);

		if(App->geometry->currentGameObject && (*iterator) == App->geometry->currentGameObject)
			DrawQuad(corners, Green);
		else
			DrawQuad(corners, Red);
	}

	if (App->geometry->currentGameObject && !App->geometry->currentGameObject->GetObjectStatic())
	{
		static float3 corners[8];
		App->geometry->currentGameObject->transform->boundingBox.GetCornerPoints(corners);
		DrawQuad(corners, Blue);
	}
}


void ModuleRenderer3D::DrawQuad(static float3* corners, Color color)
{
	glLineWidth(2.0f);
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_QUADS);

	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[3]);

	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[2]);
	glVertex3fv((GLfloat*)&corners[6]);

	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[7]);

	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[1]);
	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[2]);

	glVertex3fv((GLfloat*)&corners[3]);
	glVertex3fv((GLfloat*)&corners[7]);
	glVertex3fv((GLfloat*)&corners[6]);
	glVertex3fv((GLfloat*)&corners[2]);

	glVertex3fv((GLfloat*)&corners[0]);
	glVertex3fv((GLfloat*)&corners[4]);
	glVertex3fv((GLfloat*)&corners[5]);
	glVertex3fv((GLfloat*)&corners[1]);

	glEnd();

}

float4x4 ModuleRenderer3D::GetViewMatrix() const
{
	return currentCam->GetViewMatrix();
}

float4x4 ModuleRenderer3D::GetProjectionMatrix() const
{
	return currentCam->GetProjectionMatrix();
}
Frustum ModuleRenderer3D::GetCameraFrustrum() const
{
	return currentCam->frustum;
}


update_status ModuleRenderer3D::Save(JSON_Object* object)
{
	json_object_dotset_number(object, "render.ambientLight.red", ambient_lihgt.x);
	json_object_dotset_number(object, "render.ambientLight.green", ambient_lihgt.y);
	json_object_dotset_number(object, "render.ambientLight.blue", ambient_lihgt.z);

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Load(JSON_Object * object)
{

	ambient_lihgt.x = json_object_dotget_number(object, "render.ambientLight.red");
	ambient_lihgt.y = json_object_dotget_number(object, "render.ambientLight.green");
	ambient_lihgt.z = json_object_dotget_number(object, "render.ambientLight.blue");

	return UPDATE_CONTINUE;
}