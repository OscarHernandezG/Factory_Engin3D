#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleWindow.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "SDL/include/SDL_opengl.h"
#include "SDL/include/SDL.h"

#include <stdio.h>

#include "parson/parson.h"

#include "Primitive.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOGI("Loading Intro assets");

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
	JSON_Value *user_data = json_parse_file("user_data.json");
	user_data = json_value_init_object();

	JSON_Object* dataObj = json_object(user_data);


/*	json_object_set_boolean(dataObj, "editableValues.DemoWindow", showDemoWindow);
	json_object_set_boolean(dataObj, "editableValues.ExampeWindow", exampleWindow);
	json_object_set_boolean(dataObj, "editableValues.MathGeoLibWindow", mathGeoLibWindow);
	json_object_set_boolean(dataObj, "editableValues.randomNumberWindow", randomNumberWindow);
	json_object_set_boolean(dataObj, "editableValues.aboutWindow", aboutWindow);
	json_object_set_boolean(dataObj, "editableValues.configurationWindow", configurationWindow);
	json_object_set_boolean(dataObj, "editableValues.consoleWindow", consoleWindow);
	*/
	json_object_set_boolean(dataObj, "window.fullscreen", App->window->fullscreen);
	json_object_set_boolean(dataObj, "window.borderless", App->window->borderless);
	json_object_set_number(dataObj, "window.height", heightPos);
	json_object_set_number(dataObj, "window.width", widthPos);

	json_object_set_boolean(dataObj, "aplicationValues.IsCapped", App->toCap);
	json_object_set_number(dataObj, "aplicationValues.capFrames", App->capFrames);


	json_serialize_to_file(user_data, "user_data.json");

	json_value_free(user_data);
	   
	LOGI("Unloading Intro scene");
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

void ModuleSceneIntro::Draw3D()
{
	PrimitivePlane plane;
	plane.axis = true;
	plane.Render();

	PrimitiveCube cube;
	cube.Render();
}