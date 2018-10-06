#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049


class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool Start();
	bool CleanUp();
	
	update_status Save(JSON_Object * object);
	update_status Load(JSON_Object * object);
	
	void SetTitle(const char* title);

	void SetFullscreen(bool desktop = false);
	void SetBorderless();

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	SDL_GLContext gContext;


	bool fullscreen = false;
	bool fulldesktop = false;
	bool borderless = false;
};

#endif // __ModuleWindow_H__