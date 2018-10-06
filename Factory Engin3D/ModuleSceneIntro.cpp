#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"

#include "Primitive.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");

	bool ret = true;

	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	return ret;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;

	return status;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{	
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Draw3D(bool fill, bool wire)
{
	PrimitivePlane plane;
	plane.axis = true;
	plane.Render();

	/*PrimitiveCube cube;
	cube.fill = fill;
	cube.wire = wire;
	cube.Render();*/

	CylinderR cylinder;
	cylinder.Render();
}