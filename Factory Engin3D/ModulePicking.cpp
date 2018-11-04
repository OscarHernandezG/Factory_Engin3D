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
		float mouseX = -(1.0f - ((float(App->input->GetMouseX()) * 2.0f) / (float)App->window->width));
		float mouseY = 1.0f - ((float(App->input->GetMouseY()) * 2.0f) / (float)App->window->height);

		LineSegment line = App->camera->GetCameraFrustrum().UnProjectLineSegment(mouseX, mouseY);

		ray = RayLine(line.a, line.b);
		ray.gameObject = App->gameObject->root;
	}
	ray.Render();
	return UPDATE_CONTINUE;
}