#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"



#include "imgui-1.65\imgui.h"
//#include "imgui-1.65\examples\imgui_impl_sdl.h"
#include "imgui-1.65\examples\imgui_impl_opengl2.h"

#include <stdio.h>


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

	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__