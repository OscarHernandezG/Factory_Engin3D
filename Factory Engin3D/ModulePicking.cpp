#include "Globals.h"
#include "Application.h"
#include "ModulePicking.h"
#include "ModuleCamera3D.h"

ModulePicking::ModulePicking(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePicking::~ModulePicking()
{}

bool ModulePicking::Start()
{
	bool ret = true;

	return ret;
}

bool ModulePicking::CleanUp()
{

	return true;
}

update_status ModulePicking::Update(float dt)
{

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		ray = RayLine(App->camera->GetPos(), App->camera->GetCenter());
	}

	ray.Render();


	return UPDATE_CONTINUE;
}