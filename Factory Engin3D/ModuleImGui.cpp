#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"

#include "imgui-1.65/imgui_impl_sdl.h"
#include "imgui-1.65/imgui_impl_opengl2.h"
#include "imgui-1.65/imgui_internal.h"
#include "SDL/include/SDL_opengl.h"

#include "pcg-c-0.94/extras/entropy.h"

#include "MathGeoLib/Geometry/GeometryAll.h"

#include "GameObject.h"
#include "Geometries.h"

//RAM and CPU usage
#include "Windows.h"
#include "Psapi.h"

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

	pcg32_srandom_r(&rng, 42u, 54u);
	return ret;
}

update_status ModuleImGui::PreUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	if (exampleWindow)
		CreateExampleWindow();

	if (mathGeoLibWindow)
		CreateMGLWindow();

	if (randomNumberWindow)
		CreateRandomNumberWindow();

	if (aboutWindow)
		CreateAboutWindow();

	if (configurationWindow)
		CreateConfigWindow();

	if (consoleWindow)
		CreateConsole();

	if (transformWindow)
		CreateTransform();

	if (hierarchyWindow)
		CreateGameObjectHierarchy();

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

// Update
update_status ModuleImGui::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleImGui::PostUpdate(float dt)
{

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
	json_object_dotset_boolean(object, "editableValues.transformWindow", transformWindow);

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
	transformWindow = json_object_dotget_boolean(object, "editableValues.transformWindow");


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
		randomDoubleNum = ldexp(pcg32_random_r(&rng), -32);
		randNumTextDouble = to_string(randomDoubleNum);
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
		randomIntNum = pcg32_boundedrand_r(&rng, range);
		randomIntNum += randNum1;
		randNumTextInt = to_string(randomIntNum);

	}

	ImGui::Text("Your random number is:");
	ImGui::SameLine();
	ImGui::Text(randNumTextInt.data());

	ImGui::End();
}

void ModuleImGui::CreateAboutWindow()
{
	ImGui::Begin("About", &aboutWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::Text("Factory Engin3D");
	ImGui::Separator();
	ImGui::TextWrapped("Factory engin3D is a 3D game engine createt by Oscar Hernandez and Aleix Gabarro. Two students of CITM, UPC in Terrassa.");
	ImGui::Text("This code is written in C++");
	if (ImGui::Button("Our repository", ImVec2(150, 25)))
		ShellExecuteA(NULL, "Open", "https://github.com/OscarHernandezG/3D_Engine", NULL, NULL, SW_SHOWNORMAL);
	ImGui::Text("Did it by:");
	if (ImGui::Button("Oscar Hernandez", ImVec2(150, 25)))
		ShellExecuteA(NULL, "Open", "https://github.com/OscarHernandezG", NULL, NULL, SW_SHOWNORMAL);
	ImGui::SameLine();
	if (ImGui::Button("Aleix Gabarro", ImVec2(150, 25)))
		ShellExecuteA(NULL, "Open", "https://github.com/aleixgab", NULL, NULL, SW_SHOWNORMAL);
	ImGui::Separator();
	ImGui::Text("External libraries that we used in this engine");
	///	Todo: ask libraries their version

	//IMGUI BUTTON Link
	string nameStr = "ImGui ";
	nameStr += ImGui::GetVersion();
	if (ImGui::Button(nameStr.data(), ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "https://github.com/ocornut/imgui/releases", NULL, NULL, SW_SHOWNORMAL);

	//SDL BUTTON Link
	SDL_version current;
	SDL_VERSION(&current);
	char nameChar[25];
	sprintf_s(nameChar, 25, "SDL %d.%d.%d", current.major, current.minor, current.patch);
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "https://www.libsdl.org/download-2.0.php", NULL, NULL, SW_SHOWNORMAL);

	//OPENGL BUTTON Link
	int major = 3, minor = 1;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
	sprintf_s(nameChar, 25, "OPENGL %i.%i", major, minor);
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "https://www.opengl.org", NULL, NULL, SW_SHOWNORMAL);

	//MathGeoLib BUTTON Link
	sprintf_s(nameChar, 25, "MathGeoLib 1.5");
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "https://github.com/juj/MathGeoLib/releases/tag/v1.5", NULL, NULL, SW_SHOWNORMAL);

	//PCG BUTTON Link
	sprintf_s(nameChar, 25, "PCG 0.94");
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "http://www.pcg-random.org/download.html", NULL, NULL, SW_SHOWNORMAL);

	//Parson Link
	sprintf_s(nameChar, 25, "Parson");
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "https://github.com/kgabis/parson", NULL, NULL, SW_SHOWNORMAL);

	//Assimp Link
	sprintf_s(nameChar, 25, "Assimp 3.3");
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "http://www.assimp.org", NULL, NULL, SW_SHOWNORMAL);

	//DevIL Link
	sprintf_s(nameChar, 25, "DevIL 1.8");
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "http://openil.sourceforge.net", NULL, NULL, SW_SHOWNORMAL);

	///---------------------------------
	ImGui::Separator();
	ImGui::TextWrapped("MIT License Copyright(c) 2018 Oscar Hernandez and Aleix Gabarro	Permission is hereby granted, free of charge, to any person obtaining a copy of this software"
		"and associated documentation files(the Software), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,"
		"merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:");
	ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
	ImGui::TextWrapped("THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A"
		"PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,"
		"WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	ImGui::End();
}

void ModuleImGui::CreateConfigWindow()
{
	ImGui::Begin("Configuration", &configurationWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);


	if (ImGui::CollapsingHeader("Application"))
	{
		CreateAppHeader();
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		CreateWindowHeader();
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		CreateInputHeader();
	}

	if (ImGui::CollapsingHeader("Meshes"))
	{
		CreateMeshesHeader();
	}

	if (ImGui::CollapsingHeader("Texture"))
	{
		CreateTextureHeader();
	}

	if (ImGui::CollapsingHeader("Render"))
	{
		CreateRenderHeader();
	}

	if (ImGui::CollapsingHeader("Hardware"))
	{
		CreateHardwareHeader();
	}
	ImGui::End();

	if (warningDialoge)
	{
		ImGui::SetNextWindowSize({ 270,110 });
		ImGui::OpenPopup(" Functionality temporarily disabled");
		if (ImGui::BeginPopupModal(" Functionality temporarily disabled", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Coming soon...");
			if (ImGui::Button("Close", { 250,50 }))
				warningDialoge = false;
			ImGui::EndPopup();
		}
	}
}

void ModuleImGui::CreateConsole()
{
	ImGui::SetWindowSize({ 400,200 }, ImGuiWindowFlags_NoResize);
	ImGui::Begin("Console", &consoleWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (ImGui::Button("Clear", ImVec2(400, 20)))
		textBuff.clear();
	ImGui::Separator();

	ImGui::TextWrapped(textBuff.begin());
	if (canScroll)
		ImGui::SetScrollY(1.0f);
	canScroll = false;

	ImGui::End();
}

void ModuleImGui::CreateTransform()
{
	ImGui::SetWindowSize({ 400,200 }, ImGuiWindowFlags_NoResize);
	ImGui::Begin("Transform", &transformWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	if (App->geometry->currentMesh != nullptr)
	{
		string goName = App->geometry->currentMesh->name;
		ImGui::Text(goName.data());

		ImGui::Checkbox("Active", App->geometry->currentMesh->GetActiveReference());
		App->geometry->currentMesh->SetActive(App->geometry->currentMesh->GetActive());

		float3 position, scale, angles;
		Quat rotate;

		if (App->geometry->currentMesh->transform != nullptr)
		{
			position = App->geometry->currentMesh->transform->GetPos();
			scale = App->geometry->currentMesh->transform->scale;
			rotate = App->geometry->currentMesh->transform->GetRotation();

			if (ImGui::InputFloat3("Position", &position[0])) {
				App->geometry->currentMesh->SetPos(position);
			}

			if (ImGui::InputFloat3("Scale", &scale[0]))
				App->geometry->currentMesh->SetScale(scale);

			angles = rotate.ToEulerXYZ();

			angles[0] = math::RadToDeg(angles.x);
			angles[1] = math::RadToDeg(angles.y);
			angles[2] = math::RadToDeg(angles.z);

			if (ImGui::DragFloat3("Rotation", &angles[0]))
			{
				angles.x = math::DegToRad(angles.x);
				angles.y = math::DegToRad(angles.y);
				angles.z = math::DegToRad(angles.z);
				App->geometry->currentMesh->SetRotation(Quat::FromEulerXYZ(angles.x, angles.y, angles.z));
			}


			if (ImGui::Button("Reset", ImVec2(100, 20)))
				App->geometry->currentMesh->transform->SetIdentity();
		}
	}
	else {
		ImGui::TextWrapped("There aren't any meshes");
	}
	ImGui::End();
}

void ModuleImGui::CreateGameObjectHierarchy()
{
	ImGui::Begin("Scene", &hierarchyWindow);

	if (App->gameObject->root)
	{
		CreateGOTreeNode(App->gameObject->root);
	}
	ImGui::End();

}

void ModuleImGui::CreateGOTreeNode(GameObject* current)
{
	uint flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;// ImGuiTreeNodeFlags_OpenOnArrow;
	if (current->childs.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (current == App->geometry->currentMesh)
		flags |= ImGuiTreeNodeFlags_Selected;

	char name[80];

	string sName = current->name;

	sprintf_s(name, 80, "%s", sName.c_str());


	if (ImGui::TreeNodeEx(name, flags))
	{
		if (ImGui::IsItemClicked(0))
		{
			App->geometry->currentMesh = current;
		}

		for (list<GameObject*>::iterator childs = current->childs.begin(); childs != current->childs.end(); ++childs)
		{
			CreateGOTreeNode(*childs);
		}
		ImGui::TreePop();
	}
}



update_status ModuleImGui::CreateMainMenuBar()
{
	update_status ret = UPDATE_CONTINUE;
	if (ImGui::BeginMainMenuBar())
	{
		CreateMenu();

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

		else if (ImGui::MenuItem("Transform window", "Ctrl+T", transformWindow))
			transformWindow = !transformWindow;

		else if (ImGui::MenuItem("Console", "Ctrl+GRAVE", consoleWindow))
			consoleWindow = !consoleWindow;

		else if (ImGui::MenuItem("Close All", "Ctrl+X"))
			showDemoWindow = exampleWindow = mathGeoLibWindow = randomNumberWindow = aboutWindow = configurationWindow = consoleWindow = transformWindow = false;

		ImGui::EndMenu();
	}
}

bool ModuleImGui::CreateOptions()
{
	if (ImGui::BeginMenu("Options"))
	{

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
			transformWindow = !transformWindow;

		else if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
			consoleWindow = !consoleWindow;

		else if (App->input->GetKey(SDL_SCANCODE_X) == KEY_DOWN)
			showDemoWindow = exampleWindow = mathGeoLibWindow = randomNumberWindow = aboutWindow = configurationWindow = consoleWindow = transformWindow = false;
	}
}
//Create Menu-------------------------------------------------------------


//Create Headers----------------------------------------------------------
void ModuleImGui::CreateAppHeader()
{

	static char appName[64];
	sprintf_s(appName,64,App->aplicationName.data());
	if(ImGui::InputText("Aplication Name", appName, 64, ImGuiInputTextFlags_EnterReturnsTrue))
		App->ChangeAppName(appName);

	ImGui::SliderInt("Max Fps", &App->capFrames, 30, 144);

	ImGui::Checkbox("Cap Fps", &App->toCap);

	char graphTitle[25];

	sprintf_s(graphTitle, 25, "Framerate %.1f", App->fpsLog[App->fpsLog.size() - 1]);
	ImGui::PlotHistogram("##Framerate", &App->fpsLog[0], App->fpsLog.size(), 0, graphTitle, 0.0f, 150.0f, ImVec2(310, 100));

	sprintf_s(graphTitle, 25, "Milliseconds %.1f", App->msLog[App->msLog.size() - 1]);
	ImGui::PlotHistogram("##Milliseconds", &App->msLog[0], App->msLog.size(), 0, graphTitle, 0.0f, 40.0f, ImVec2(310, 100));

	sprintf_s(graphTitle, 25, "RAM Usage %.1f", ramLog[ramLog.size() - 1]);
	ImGui::PlotHistogram("##RAM", &ramLog[0], ramLog.size(), 0, graphTitle, 0.0f, 125.0f, ImVec2(310, 100));
}

void ModuleImGui::CreateWindowHeader()
{
	if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
	{
		App->window->fulldesktop = false;
		App->window->SetFullscreen();
	}
	ImGui::SameLine();
	if (ImGui::Checkbox("Borderless", &App->window->borderless))
	{
		App->window->SetBorderless();
	}
	if (ImGui::Checkbox("Full Desktop", &App->window->fulldesktop))
	{
		//App->window->fullscreen = false;
		//App->window->SetFullscreen(true);
		App->window->fulldesktop = false;
		warningDialoge = true;
	}
	if (ImGui::SliderFloat("Brightness", &brightnessPos, 0.0f, 1.0f))
	{
		SDL_SetWindowBrightness(App->window->window, brightnessPos);
	}
	if (ImGui::SliderInt("Width", &widthPos, 640, 1920))
	{
		//SDL_SetWindowSize(App->window->window, widthPos, heightPos);
		warningDialoge = true;
	}
	if (ImGui::SliderInt("Height", &heightPos, 480, 1080))
	{
		//SDL_SetWindowSize(App->window->window, widthPos, heightPos);
		warningDialoge = true;

	}
}

void ModuleImGui::CreateInputHeader()
{
	ImGui::Text("Mouse position: (%i,%i)", App->input->GetMouseX(), App->input->GetMouseY());
	ImGui::Text("Mouse motion: (%i,%i)", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
	
	ImGui::Text("Is mouse pressed: %s", App->input->GetIsMousePressed() == true ? "Yes" : "No");
	ImGui::Text("Is any key pressed: %s", App->input->GetIsKeyPressed() == true ? "Yes" : "No");
}

void ModuleImGui::CreateMeshesHeader()
{
	if (App->geometry->currentMesh != nullptr)
	{
		Geometry* goGeometry = (Geometry*)App->geometry->currentMesh->GetComponent(ComponentType::ComponentType_GEOMETRY);
		if (goGeometry != nullptr)
		{
			if (goGeometry->GetType() == PrimitiveTypes::Primitive_Mesh)
			{

				Mesh* mesh = (Mesh*)goGeometry;

				ImGui::Text("Total vertex: %i", mesh->buffer.vertex.size);
				ImGui::Text("Total faces: %i", App->geometry->numFaces);
				if (ImGui::Button("Remove Mesh", { 125,25 }))
				{
					Geometry* mesh = (Geometry*)App->geometry->currentMesh->GetComponent(ComponentType::ComponentType_GEOMETRY);
					App->geometry->currentMesh->RemoveComponent(mesh);
				}
			}
		}
		else
			ImGui::TextWrapped("There aren't any meshes");

	}
	else
		ImGui::TextWrapped("There aren't any meshes");
	
}

void ModuleImGui::CreateTextureHeader()
{
	if (App->geometry->textureID != 0)
	{
		Geometry* currentGeometry = (Geometry*)App->geometry->currentMesh->GetComponent(ComponentType_GEOMETRY);
		if (currentGeometry != nullptr)
			if (currentGeometry->GetType() == PrimitiveTypes::Primitive_Mesh)
			{
				Mesh* mesh = (Mesh*)currentGeometry;

			ImGui::Text("Texture id: %i", App->geometry->textureID);
			// TODO
			//ImGui::Text("Texture used by %i meshes", mesh->buffers.size());
			ImGui::Text("UV Preview");
			ImGui::Separator();
			ImGui::Image((void*)App->geometry->textureID, { 200,200 });
			if (ImGui::Button("Remove Texture", { 125,25 }))
			{
				glDeleteTextures(1, &App->geometry->textureID);
				App->geometry->textureID = 0;
			}
		}
	}
	else
	{
		ImGui::TextWrapped("There isn't any texture");
	}
}

void ModuleImGui::CreateRenderHeader()
{
	bool fill = false;
	fill = ImGui::Checkbox("Fill primitives", &App->renderer3D->isFill);
	ImGui::Checkbox("Wireframe", &App->renderer3D->isWire);

	if (!App->renderer3D->isFill && !App->renderer3D->isWire)
	{
		if (fill)
			App->renderer3D->isWire = true;
		else
			App->renderer3D->isFill = true;
	}

	if (ImGui::ColorPicker3("Light Model Ambient", (float*)&App->renderer3D->ambient_lihgt))
		App->renderer3D->SetLightAmbient();
	
}

void ModuleImGui::CreateHardwareHeader()
{
	ImVec4 color(1.0f, 1.0f, 0.1f, 1.0f);

	//CPU--------------------------------------------------------
	CreateCPUInfo(color);

	ImGui::Separator();

	//GPU--------------------------------------------------------
	CreateGPUInfo(color);
}

//Create Headers----------------------------------------------------------


void ModuleImGui::CreateCPUInfo(ImVec4 color)
{
	//CPU--------------------------------------------------------
	ImGui::Text("CPUs: "); ImGui::SameLine();
	ImGui::TextColored(color, "%i (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

	//RAM--------------------------------------------------------
	ImGui::Text("System RAM: "); ImGui::SameLine();
	ImGui::TextColored(color, "%.2fGB", (float)SDL_GetSystemRAM() / 1024);

	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	float totalRamInUse = physMemUsed / 1024;
	totalRamInUse /= 1024;
	totalRamInUse /= 1024;

	ImGui::Text("Total RAM in use:"); ImGui::SameLine();
	ImGui::TextColored(color, "%.2f GB", totalRamInUse);



	ImGui::Text("RAM used by Factory Engin3D:"); ImGui::SameLine();
	ImGui::TextColored(color, "%.2f MB", ramLog.back());


	//CAP--------------------------------------------------------
	ImGui::Text("Caps: "); ImGui::SameLine();
	string caps("");
	CheckCaps(&caps);

	ImGui::TextColored(color, caps.data());
}

void ModuleImGui::CreateGPUInfo(ImVec4 color)
{
	//GPU--------------------------------------------------------
	const GLubyte* gpuInfo = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);

	ImGui::Text("GPU info:"); ImGui::SameLine();
	ImGui::TextColored(color, (char*)gpuInfo); ImGui::SameLine();
	ImGui::TextColored(color, (char*)renderer);


	//VRAM-------------------------------------------------------
	GLint nTotalMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
		&nTotalMemoryInKB);

	GLint nCurAvailMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
		&nCurAvailMemoryInKB);

	// TODO AMD/ATI VRAM
	//nCurAvailMemoryInKB = 0;
	//glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI,
	//	&nCurAvailMemoryInKB);
 
	ImGui::Text("VRAM total: "); ImGui::SameLine();
	ImGui::TextColored(color, "%.2f MB", (float)nTotalMemoryInKB / 1024);

	ImGui::Text("VRAM available: "); ImGui::SameLine();
	ImGui::TextColored(color, "%.2f MB", (float)nCurAvailMemoryInKB / 1024);

	ImGui::Text("VRAM in use: "); ImGui::SameLine();
	ImGui::TextColored(color, "%.2f MB", ((float)nTotalMemoryInKB / 1024) - (float)nCurAvailMemoryInKB / 1024);
}

void ModuleImGui::CheckCaps(string* caps)
{
	if (SDL_Has3DNow())
		*caps += "3Dnow, ";
	if (SDL_HasAVX())
		*caps += "AVX, ";
	if (SDL_HasAVX2())
		*caps += "AVX2, ";
	if (SDL_HasAltiVec())
		*caps += "AltiVec, ";
	if (SDL_HasMMX())
		*caps += "MMX, ";
	if (SDL_HasRDTSC())
		*caps += "RDTSC , ";
	if (SDL_HasSSE())
		*caps += "SSE, ";
	if (SDL_HasSSE2())
		*caps += "SSE2, ";
	if (SDL_HasSSE3())
		*caps += "SSE3, ";
	if (SDL_HasSSE41())
		*caps += "SSE4.1, ";
	if (SDL_HasSSE42())
		*caps += "SSE4.2, ";

	*caps = caps->substr(0, caps->size() - 2);
}

void ModuleImGui::LogConsole(const char * consoleText)
{
	textBuff.appendf(consoleText);
	canScroll = true;
}


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

