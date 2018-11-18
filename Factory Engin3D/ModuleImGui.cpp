#include "Application.h"
#include "ModuleImGui.h"

#include "imgui-1.65/imgui_impl_sdl.h"
#include "imgui-1.65/imgui_impl_opengl2.h"
#include "imgui-1.65/imgui_internal.h"
#include "imgui-1.65/imgui_dock.h"

#include "MathGeoLib/Geometry/GeometryAll.h"

#include "GameObject.h"
#include "Geometries.h"


//RAM and CPU usage
#include "Windows.h"
#include "Psapi.h"

using namespace std;

ModuleImGui::ModuleImGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleImGui::~ModuleImGui()
{}

// Load assets
bool ModuleImGui::Start()
{
	LOG("Loading Intro assets");

	bool ret = true;

	//--------------------------
	// Window position
	aboutPos = float2(955.0f, 520.0f);
	configurationPos = float2(355.0f, 575.0f);
	scenePos = float2(0.0f, 18.0f);
	playPos = float2(550.0f, 25.0f);
	playCountPos = float2(-120.0f, 0.0f);
	inspectorPos = float2(1026.0f, 18.0f);
	dockWindowPos = float2(0.0f, 592.0f);

	// Window sizes
	aboutSize = float2(325.0f, 340.0f);
	configurationSize = float2(600.0f, 287.0f);
	sceneSize = float2(254.0f, 575.0f);
	playSize = float2(185.0f, 40.0f);
	playCountSize = float2(205.0f, 0.0f);
	inspectorSize = float2(254.0f, 852.0f);
	dockWindowSize = float2(1026.0f, 270.0f);
	//--------------------------

	ImGui::InitDock();
	contRefresh.Start();
	RefreshAssets(".\\Assets\\");

	return ret;
}

update_status ModuleImGui::PreUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	update_status status = UPDATE_CONTINUE;

	// Calc windows scale
	int w;	int h;
	SDL_GetWindowSize(App->window->window, &w, &h);
	float2 scale((float)w / SCREEN_WIDTH, (float)h / SCREEN_HEIGHT);

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	if (exampleWindow)
		CreateExampleWindow();

	if (mathGeoLibWindow)
		CreateMGLWindow();

	if (randomNumberWindow)
		CreateRandomNumberWindow();

	if (aboutWindow)
		CreateAboutWindow(scale);

	if (configurationWindow)
		CreateConfigWindow(scale);

	if (inspectorWindow)
		CreateInspector(scale);

	if (hierarchyWindow)
		CreateGameObjectHierarchy(scale);

	if (saveScenePopUp)
	{
		ImGui::OpenPopup("Save scene");
		saveScenePopUp = false;
	}

	if (ImGui::BeginPopup("Save scene", ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGui::SetWindowPos({ float(450.0f * scale.x), float(300.0f * scale.y) });
		static char scene[64];
		sprintf_s(scene, 64, sceneName.data());

		ImGui::Text("Introduce the name of the scene");
		if (ImGui::InputText("", scene, 64))
		{
			sceneName = scene;
		}
		ImGui::SameLine();
		if (ImGui::Button("Save", { 50,25 }) || App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			App->gameObject->SaveScene(sceneName.data());
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Cancel", { 50,25 }))
		{
			sceneName = "scene";
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
//------------------
	if (assetsWindow)
	{
		if (ImGui::Begin("Dock windows", &assetsWindow, ImGuiWindowFlags_NoTitleBar))
		{
			SetWindowDim(dockWindowPos, dockWindowSize, scale);
			ImGui::BeginDockspace();
			if (consoleWindow)
				CreateConsole(scale);

			CreateAssetsWindow(scale);
			ImGui::EndDockspace();
		}
		ImGui::End();
	}
//------------------
	
		CreateGameManager(scale);

	status = CreateMainMenuBar();

	created = true;

	return status;
}

// Load assets
bool ModuleImGui::CleanUp()
{
	textBuff.clear();
	LOG("Unloading Intro scene");
	return true;
}

update_status ModuleImGui::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);


	PROCESS_MEMORY_COUNTERS counters;
	GetProcessMemoryInfo(GetCurrentProcess(), &counters, sizeof(counters));

	float ramInUse = counters.WorkingSetSize;
	ramInUse /= 1024;
	ramInUse /= 1024;

	ramLog.push_back(ramInUse);
	if (ramLog.size() > 75)
	{
		ramLog.erase(ramLog.begin());
	}

	return UPDATE_CONTINUE;
}

void ModuleImGui::DrawUI()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

update_status ModuleImGui::Save(JSON_Object* object)
{

	json_object_dotset_boolean(object, "editableValues.DemoWindow", showDemoWindow);
	json_object_dotset_boolean(object, "editableValues.ExampeWindow", exampleWindow);
	json_object_dotset_boolean(object, "editableValues.MathGeoLibWindow", mathGeoLibWindow);
	json_object_dotset_boolean(object, "editableValues.randomNumberWindow", randomNumberWindow);
	json_object_dotset_boolean(object, "editableValues.aboutWindow", aboutWindow);
	json_object_dotset_boolean(object, "editableValues.configurationWindow", configurationWindow);
	json_object_dotset_boolean(object, "editableValues.consoleWindow", consoleWindow);
	json_object_dotset_boolean(object, "editableValues.inspectorWindow", inspectorWindow);

	json_object_dotset_number(object, "window.height", heightPos);
	json_object_dotset_number(object, "window.width", widthPos);

	return UPDATE_CONTINUE;
}

update_status ModuleImGui::Load(JSON_Object * object)
{
	showDemoWindow = json_object_dotget_boolean(object, "editableValues.DemoWindow");
	exampleWindow = json_object_dotget_boolean(object, "editableValues.ExampeWindow");
	mathGeoLibWindow = json_object_dotget_boolean(object, "editableValues.MathGeoLibWindow");
	randomNumberWindow = json_object_dotget_boolean(object, "editableValues.randomNumberWindow");
	aboutWindow = json_object_dotget_boolean(object, "editableValues.aboutWindow");
	configurationWindow = json_object_dotget_boolean(object, "editableValues.configurationWindow");
	consoleWindow = json_object_dotget_boolean(object, "editableValues.consoleWindow");
	inspectorWindow = json_object_dotget_boolean(object, "editableValues.inspectorWindow");


	heightPos = json_object_dotget_number(object, "window.height");
	widthPos = json_object_dotget_number(object, "window.width");

	SDL_SetWindowSize(App->window->window, widthPos, heightPos);

	return UPDATE_CONTINUE;
}

bool ModuleImGui::IsAnyWindowHovered()
{
	return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

//Create Windows----------------------------------------------------------
void ModuleImGui::CreateExampleWindow()
{
	ImGui::Begin("Hello World!", &exampleWindow);				// Create a window called "Hello, world!" and append into it.

	ImGui::Checkbox("Demo Window", &showDemoWindow);			// Edit bools storing our window open/close state
	ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);				// Edit 1 float using a slider from 0.0f to 1.0f    
	ImGui::ColorEdit3("clear color", (float*)&clear_color);		// Edit 3 floats representing a color
	ImGui::ColorPicker4("test", (float*)&clear_color);

	if (ImGui::Button("Button"))								// Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;

	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void ModuleImGui::CreateMGLWindow()
{
	ImGui::Begin("MathGeoLib", &mathGeoLibWindow);
	ImGui::Text("Move the sliders to move the sphere in the X axis.");
	ImGui::Text("Both spheres are 1 unit in radius");

	ImGui::NewLine();

	ImGui::SliderFloat("Sphere 1 position", &sphere1Pos, 0, 5);
	ImGui::SliderFloat("Sphere 2 positon", &sphere2Pos, 0, 5);

	Sphere sphere1;
	sphere1.pos = { sphere1Pos,0,0 };
	sphere1.r = 1;

	Sphere sphere2;
	sphere2.pos = { sphere2Pos,0,0 };
	sphere2.r = 1;

	spheresColliding = sphere1.Intersects(sphere2);


	ImGui::NewLine();
	if (spheresColliding)
	{
		ImGui::Text("The spheres are colliding");
	}
	else
	{
		ImGui::Text("The spheres are not colliding");
	}
	ImGui::End();
}

void ModuleImGui::CreateRandomNumberWindow()
{
	ImGui::Begin("PCG", &randomNumberWindow);
	ImGui::Text("Create random numbers.");

	if (ImGui::Button("Get a random number (0.0-1.0)", ImVec2(300, 50)))
	{
		//randomDoubleNum = ldexp(pcg32_random_r(&rng), -32);
		//randNumTextDouble = to_string(randomDoubleNum);
	}

	if (randomDoubleNum > -1)
	{
		ImGui::Text("Your random number is:");
		ImGui::SameLine();
		ImGui::Text(randNumTextDouble.data());
	}


	ImGui::Text("Introduce min value:"); ImGui::SameLine();
	isMinSelected = ImGui::InputInt("", &randNum1);

	ImGui::Text("Introduce max value:"); ImGui::SameLine();
	isMaxSelected = ImGui::InputInt(" ", &randNum2);

	if (randNum1 > randNum2)
	{
		if (isMinSelected)
			randNum2 = randNum1;
		else if (isMaxSelected)
			randNum1 = randNum2;
	}

	string buttonText = "Get a random number between " + to_string(randNum1) + " and " + to_string(randNum2);
	if (ImGui::Button(buttonText.data(), ImVec2(400, 50)))
	{
		int range = randNum2 - randNum1 + 1;
		//randomIntNum = pcg32_boundedrand_r(&rng, range);
		randomIntNum += randNum1;
		randNumTextInt = to_string(randomIntNum);

	}

	ImGui::Text("Your random number is:");
	ImGui::SameLine();
	ImGui::Text(randNumTextInt.data());

	ImGui::End();
}

void ModuleImGui::SetWindowDim(float2 &pos, float2 &size, float2 &scale, bool gameWindow)
{


	float2 realPos = pos.Mul(scale);
	float2 realSize = size.Mul(scale);

	if (gameWindow)
	{
		if (App->time->gameState != GameState_NONE)
		{
			realSize = size + playCountSize;
			realPos += playCountPos.Mul(scale);
		}
		else 
			realSize = size;
	}
	ImGui::SetWindowPos({ realPos.x, realPos.y });
	ImGui::SetWindowSize({ realSize.x, realSize.y });
}

update_status ModuleImGui::CreateMainMenuBar()
{
	update_status ret = UPDATE_CONTINUE;
	if (ImGui::BeginMainMenuBar())
	{
		CreateMenu();

		CreateDebugMenu();

		CheckShortCuts();

		if (CreateOptions())
			ret = UPDATE_STOP;
	}
	ImGui::EndMainMenuBar();

	return ret;
}

//Create Windows----------------------------------------------------------


//Create Menu-------------------------------------------------------------
void ModuleImGui::CreateMenu()
{
	if (ImGui::BeginMenu("Menu"))
	{

		if (ImGui::MenuItem("About", "Ctrl+A", aboutWindow))
			aboutWindow = !aboutWindow;

		else if (ImGui::MenuItem("Configuration", "Ctrl+C", configurationWindow))
			configurationWindow = !configurationWindow;

		else if (ImGui::MenuItem("Demo window", "Ctrl+D", showDemoWindow))
			showDemoWindow = !showDemoWindow;

		else if (ImGui::MenuItem("Example window", "Ctrl+E", exampleWindow))
			exampleWindow = !exampleWindow;

		else if (ImGui::MenuItem("Math window", "Ctrl+M", mathGeoLibWindow))
			mathGeoLibWindow = !mathGeoLibWindow;

		else if (ImGui::MenuItem("Random number window", "Ctrl+R", randomNumberWindow))
			randomNumberWindow = !randomNumberWindow;

		else if (ImGui::MenuItem("Inspector window", "Ctrl+T", inspectorWindow))
			inspectorWindow = !inspectorWindow;

		else if (ImGui::MenuItem("Hierarchy", "Ctrl+H", hierarchyWindow))
			hierarchyWindow = !hierarchyWindow;

		else if (ImGui::MenuItem("Console", "Ctrl+GRAVE", consoleWindow))
			consoleWindow = !consoleWindow;

		else if (ImGui::MenuItem("Close All", "Ctrl+X"))
			showDemoWindow = exampleWindow = mathGeoLibWindow = randomNumberWindow = aboutWindow = configurationWindow = consoleWindow = inspectorWindow = hierarchyWindow = assetsWindow = false;

		ImGui::EndMenu();
	}
}

bool ModuleImGui::CreateOptions()
{
	if (ImGui::BeginMenu("Options"))
	{
		if (ImGui::MenuItem("Save Scene"))
			saveScenePopUp = true;

		if (ImGui::MenuItem("Load Scene"))
			App->gameObject->LoadScene();

		if (ImGui::MenuItem("Save", "Ctrl+S"))
			App->canSave = true;

		else if (ImGui::MenuItem("Load", "Ctrl+L"))
			App->canLoad = true;

		else if (ImGui::MenuItem("Exit", "ESC"))
			exitOption = true;

		ImGui::EndMenu();
	}
	return exitOption;
}

void ModuleImGui::CheckShortCuts()
{
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
			aboutWindow = !aboutWindow;

		else if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			configurationWindow = !configurationWindow;

		else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
			showDemoWindow = !showDemoWindow;

		else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			exampleWindow = !exampleWindow;

		else if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
			App->canLoad = true;

		else if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
			mathGeoLibWindow = !mathGeoLibWindow;

		else if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
			App->canSave = true;

		else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			randomNumberWindow = !randomNumberWindow;

		else if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN)
			inspectorWindow = !inspectorWindow;

		else if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
			hierarchyWindow = !hierarchyWindow;

		else if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
			consoleWindow = !consoleWindow;

		else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
			showDemoWindow = exampleWindow = mathGeoLibWindow = randomNumberWindow = aboutWindow = configurationWindow = consoleWindow = inspectorWindow = hierarchyWindow = assetsWindow = false;
	}
}

void ModuleImGui::CreateDebugMenu()
{
	if (ImGui::BeginMenu("Debug"))
	{

		if (ImGui::MenuItem("AABB & quadtree", "1", App->renderer3D->debugQuad))
			App->renderer3D->debugQuad = !App->renderer3D->debugQuad;

		ImGui::MenuItem("Create random empty object", "2");

		if (ImGui::MenuItem("Camera culling", "3", App->renderer3D->cameraCulling))
			App->renderer3D->cameraCulling = !App->renderer3D->cameraCulling;

		ImGui::EndMenu();
	}
}
//Create Menu-------------------------------------------------------------

void ModuleImGui::ResizeImGui(float2 scale)
{
	ImGuiContext* context = ImGui::GetCurrentContext();
	for (ImVector<ImGuiWindow*>::iterator currentWindow = context->Windows.begin(); currentWindow != context->Windows.end(); ++currentWindow)
	{
		(*currentWindow)->Pos.x *= scale.x;
		(*currentWindow)->Pos.y *= scale.y;

		(*currentWindow)->SizeFull.x *= scale.x;
		(*currentWindow)->SizeFull.y *= scale.y;

		(*currentWindow)->Size.x *= scale.x;
		(*currentWindow)->Size.y *= scale.y;

	}
}