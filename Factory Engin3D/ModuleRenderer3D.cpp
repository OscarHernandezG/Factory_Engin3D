#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "glew-2.1.0/include/GL/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


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
		
		SetLightAmbient({ 1.0f, 1.0f, 1.0f });
		
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


	const int eight = 80;
	GLubyte checkImage[eight][eight][4];
	for (int i = 0; i < eight; i++) {
		for (int j = 0; j < eight; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &buffer_id);
	glBindTexture(GL_TEXTURE_2D, buffer_id);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, eight, eight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	return true;
}


// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// Light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{

	// 1. Draw geometry
	App->geometry->Draw3D(isFill, isWire);

	// 2. Debug geometry
	//TODO

	// 3. Draw UI
	App->gui->DrawUI();

	// 4. Swap buffers
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
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
}

void ModuleRenderer3D::SetLightAmbient(float3 rgb)
{
	GLfloat LightModelAmbient[] = { rgb.x, rgb.y, rgb.z, 1.0f };
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
