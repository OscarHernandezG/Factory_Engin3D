#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;

		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}	


	return ret;
}

bool ModuleWindow::Start()
{
	return true;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::SetFullscreen(bool desktop) const
{
	if (!fullscreen && !fulldesktop)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_SHOWN);

		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	}
	else
	{
		if(desktop)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

		else
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

		SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

//void ModuleWindow::SetMaximize()
//{
//	//SDL_MaximizeWindow(window);
//
//	//SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
//}

void ModuleWindow::SetBorderless() const
{
	if (!borderless)
		SDL_SetWindowBordered(window, SDL_TRUE);
	
	else
		SDL_SetWindowBordered(window, SDL_FALSE);

	//SDL_MaximizeWindow(window);

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

update_status ModuleWindow::Save(JSON_Object* object)
{

	json_object_dotset_boolean(object, "window.fullscreen", fullscreen);
	json_object_dotset_boolean(object, "window.borderless", borderless);
	json_object_dotset_boolean(object, "window.fulldesktop", fulldesktop);

	return UPDATE_CONTINUE;
}

update_status ModuleWindow::Load(JSON_Object * object)
{

	fullscreen = json_object_dotget_boolean(object, "window.fullscreen");
	borderless = json_object_dotget_boolean(object, "window.borderless");
	fulldesktop = json_object_dotget_boolean(object, "window.fulldesktop");

	if (fullscreen || fulldesktop)
		SetFullscreen();
	if (borderless)
		SetBorderless();

	return UPDATE_CONTINUE;
}