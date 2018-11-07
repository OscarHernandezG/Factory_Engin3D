#include "Globals.h"
#include "Application.h"
#include "ModuleTimeManager.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>

ModuleTimeManager::ModuleTimeManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleTimeManager::~ModuleTimeManager()
{
}

// Called before render is available
bool ModuleTimeManager::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleTimeManager::Start()
{
	return true;
}

// Called before quitting
bool ModuleTimeManager::CleanUp()
{
	return true;
}


update_status ModuleTimeManager::Save(JSON_Object* object)
{

	return UPDATE_CONTINUE;
}

update_status ModuleTimeManager::Load(JSON_Object * object)
{

	return UPDATE_CONTINUE;
}