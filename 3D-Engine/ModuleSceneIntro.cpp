#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

#include "imgui-1.65\imgui.cpp"
#include "imgui-1.65\imstb_rectpack.h"
#include "imgui-1.65\imstb_textedit.h"
#include "imgui-1.65\imstb_truetype.h"
#include "imgui-1.65\examples\imgui_impl_opengl2.h"

#include "imgui-1.65\imgui_demo.cpp"
#include "imgui-1.65\imgui_draw.cpp"
#include "imgui-1.65\imgui_widgets.cpp"


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

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	ImGui::Text("Hello world %d", 123);

	//if (ImGui::Button("Save"))
	//{
	//	// do stuff
	//}
//	ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

