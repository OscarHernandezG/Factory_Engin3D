#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

#include "glew-2.1.0/include/GL/glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew-2.1.0/lib/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOGI("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOGE("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		LOGE("Glew could not init %s\n", glewGetErrorString(error));
	}
	else
		LOGE("Using Glew %s", glewGetString(GLEW_VERSION));

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
			LOGE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOGE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOGE("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
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

//		glEnable(GL_TEXTURE_2D);
//		glShadeModel(GL_SMOOTH);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	return ret;
}

bool ModuleRenderer3D::Start()
{
	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	float vertexQuad[]
	{
	-0.5f,- 0.5f, -0.5f,//a
	 0.5f, -0.5f, -0.5f,//b
	-0.5f,  0.5f,  0.5f,//c
	-0.5f,  0.5f,  0.5f,//c
	 0.5f, -0.5f,  0.5f,//b
	 0.5f,  0.5f,  0.5f,//d
	  
	 0.5f,  0.5f, -0.5f,//d
	 0.5f, -0.5f, -0.5f,//b
	 0.5f, -0.5f,  0.5f,//f
	 0.5f, -0.5f,  0.5f,//f
	 0.5f,  0.5f,  0.5f,//h
	 0.5f,  0.5f, -0.5f,//d
			  
	-0.5f,  0.5f, -0.5f,//c
	 0.5f,  0.5f, -0.5f,//d
	-0.5f,  0.5f,  0.5f,//g
	-0.5f,  0.5f,  0.5f,//g
	 0.5f,  0.5f, -0.5f,//d
	 0.5f,  0.5f,  0.5f,//h
			  
	-0.5f,  0.5f,  0.5f,//g
	-0.5f, -0.5f,  0.5f,//e
	-0.5f, -0.5f, -0.5f,//a
	-0.5f, -0.5f, -0.5f,//a
	-0.5f,  0.5f, -0.5f,//c
	-0.5f,  0.5f,  0.5f,//g
			  
	-0.5f, -0.5f, -0.5f,//a
	-0.5f, -0.5f,  0.5f,//e
	 0.5f, -0.5f,  0.5f,//f
	 0.5f, -0.5f,  0.5f,//f
	 0.5f, -0.5f, -0.5f,//b
	-0.5f, -0.5f, -0.5f,//a
				  
	 0.5f,  0.5f,  0.5f,//h
	 0.5f, -0.5f,  0.5f,//f
	-0.5f, -0.5f,  0.5f,//e
	-0.5f, -0.5f,  0.5f,//e
	-0.5f,  0.5f,  0.5f,//g
	 0.5f,  0.5f,  0.5f,//h				  
	};

	//Create Quad
	glGenBuffers(1, (GLuint*)&(my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 108, vertexQuad, GL_STATIC_DRAW); 
	// 108 = All vertex positions (36 * 3) 36 = numsOfVertex and 3 = pos x-y-z

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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// 1. Draw geometry
	App->sceneIntro->Draw3D();



	if (isFill)
	{
		DrawQuadTriangles();
		DrawQuadVertex();
	}
	if (isWire)
	{
		DrawCubeWireframe();
		DrawQuadVertexWireframe();
	}
	// 2. Debug geometry
	//TODO

	// 3. Draw UI
	App->sceneIntro->DrawUI();

	// 4. Swap buffers
	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOGI("Destroying 3D Renderer");

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

void ModuleRenderer3D::DrawQuadTriangles()
{
		glLineWidth(2.0f);
	glRotatef(0.1f, 1.0f, 1.0f, 0.0f);
	glTranslatef(-4.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glVertex3f(-0.5f, -0.5f, -0.5f);//a
	glVertex3f(0.5f, -0.5f, -0.5f);//b
	glVertex3f(-0.5f,  0.5f,  0.5f);//c
	glVertex3f(-0.5f,  0.5f,  0.5f);//c
	glVertex3f(0.5f, -0.5f,  0.5f);//b
	glVertex3f(0.5f,  0.5f,  0.5f);//d

	glVertex3f(0.5f, 0.5f, -0.5f);//d
	glVertex3f(0.5f, -0.5f, -0.5f);//b
	glVertex3f(0.5f, -0.5f, 0.5f);//f
	glVertex3f(0.5f, -0.5f, 0.5f);//f
	glVertex3f(0.5f, 0.5f, 0.5f);//h
	glVertex3f(0.5f, 0.5f, -0.5f);//d

	glVertex3f(-0.5f, 0.5f, -0.5f);//c
	glVertex3f(0.5f, 0.5f, -0.5f);//d
	glVertex3f(-0.5f, 0.5f, 0.5f);//g
	glVertex3f(-0.5f, 0.5f, 0.5f);//g
	glVertex3f(0.5f, 0.5f, -0.5f);//d
	glVertex3f(0.5f, 0.5f, 0.5f);//h

	glVertex3f(-0.5f, 0.5f, 0.5f);//g
	glVertex3f(-0.5f, -0.5f, 0.5f);//e
	glVertex3f(-0.5f, -0.5f, -0.5f);//a
	glVertex3f(-0.5f, -0.5f, -0.5f);//a
	glVertex3f(-0.5f, 0.5f, -0.5f);//c
	glVertex3f(-0.5f, 0.5f, 0.5f);//g

	glVertex3f(-0.5f, -0.5f, -0.5f);//a
	glVertex3f(-0.5f, -0.5f, 0.5f);//e
	glVertex3f(0.5f, -0.5f, 0.5f);//f
	glVertex3f(0.5f, -0.5f, 0.5f);//f
	glVertex3f(0.5f, -0.5f, -0.5f);//b
	glVertex3f(-0.5f, -0.5f, -0.5f);//a

	glVertex3f(0.5f, 0.5f, 0.5f);//h
	glVertex3f(0.5f, -0.5f, 0.5f);//f
	glVertex3f(-0.5f, -0.5f, 0.5f);//e
	glVertex3f(-0.5f, -0.5f, 0.5f);//e
	glVertex3f(-0.5f, 0.5f, 0.5f);//g
	glVertex3f(0.5f, 0.5f, 0.5f);//h

	glEnd();

	glLineWidth(1.0f);

}

void ModuleRenderer3D::DrawQuadVertex()
{
	glTranslatef(2.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 36); //36 = numVertices

	
	glDisableClientState(GL_VERTEX_ARRAY);
	glTranslatef(-2.0f, 0.0f, 0.0f);

}

void ModuleRenderer3D::DrawQuadVertexWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 1, 0);


	glTranslatef(2.0f, 0.0f, 0.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_TRIANGLES, 0, 36); //36 = numVertices


	glDisableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1, 1, 1);
	glTranslatef(-2.0f, 0.0f, 0.0f);

}

void ModuleRenderer3D::DrawQuadIndices()
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

void ModuleRenderer3D::DrawCubeWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 1, 0);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, my_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1, 1, 1);


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

