#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"

#include "Primitive.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


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

	//warrior = LoadMesh();
	
	frust = new PrimitiveFrustum(0.5f, 1.5f, { 2,0,0 });
	cube = new PrimitiveCube({ 0,2,0 }, 1, 1, 1);

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
	delete frust;
	delete cube;

	return true;
}
// Update
update_status ModuleSceneIntro::Update(float dt)
{
	warrior.Render();
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}