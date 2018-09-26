#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleWindow.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "SDL/include/SDL_opengl.h"
#include "SDL/include/SDL.h"

#include <stdio.h>

#include "pcg-c-0.94/extras/entropy.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>


#include "MathGeoLib/Geometry/GeometryAll.h"

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

	SDL_GLContext gl_context = SDL_GL_CreateContext(App->window->window);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, gl_context);
	ImGui_ImplOpenGL2_Init();

	ImGui::StyleColorsDark();

	pcg32_srandom_r(&rng, 42u, 54u);

	return ret;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();


	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	static float f = 0.0f;
	static int counter = 0;

	if (showDemoWindow)
		ImGui::ShowDemoWindow(&showDemoWindow);

	if (exampleWindow)
	{
		ImGui::Begin("Hello World!", &exampleWindow);				// Create a window called "Hello, world!" and append into it.

		ImGui::Checkbox("Demo Window", &showDemoWindow);			// Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);				// Edit 1 float using a slider from 0.0f to 1.0f    
		ImGui::ColorEdit3("clear color", (float*)&clear_color);		// Edit 3 floats representing a color

		if (ImGui::Button("Button"))								// Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	if (mathGeoLibWindow)
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

	if (randomNumberWindow)
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
		isMinSelected = ImGui::InputInt("", &num1);

		ImGui::Text("Introduce max value:"); ImGui::SameLine();
		isMaxSelected = ImGui::InputInt(" ", &num2);

		if (num1 > num2)
		{
			if (isMinSelected)
				num2 = num1;
			else if (isMaxSelected)
				num1 = num2;
		}

		string buttonText = "Get a random number between " + to_string(num1) + " and " + to_string(num2);
		if (ImGui::Button(buttonText.data(), ImVec2(400, 50)))
		{
			int range = num2 - num1 + 1;
			randomIntNum = pcg32_boundedrand_r(&rng, range);
			randomIntNum += num1;
			randNumTextInt = to_string(randomIntNum);

		}

		ImGui::Text("Your random number is:");
		ImGui::SameLine();
		ImGui::Text(randNumTextInt.data());


		ImGui::End();
	}

	if (aboutWindow)
	{
		ImGui::Begin("About", &aboutWindow);
		ImGui::Text("Factory Engin3D");
		ImGui::Separator();
		ImGui::TextWrapped("This is an incredible description ^^");
		if (ImGui::Button("Our repository", ImVec2(150, 25)))
			ShellExecuteA(NULL, "Open", "https://github.com/OscarHernandezG/3D_Engine", NULL, NULL, SW_SHOWNORMAL);
		ImGui::Text("Did it by:");
		if (ImGui::Button("Oscar Hernandez", ImVec2(150, 25)))
			ShellExecuteA(NULL, "Open", "https://github.com/OscarHernandezG", NULL, NULL, SW_SHOWNORMAL);
		ImGui::SameLine();
		if (ImGui::Button("Aleix Gabarro", ImVec2(150, 25)))
			ShellExecuteA(NULL, "Open", "https://github.com/aleixgab", NULL, NULL, SW_SHOWNORMAL);
		ImGui::Separator();
		ImGui::Text("Libraries that we used in this engine");
		///	Todo: ask libraries their version
		ImGui::Text("MathGeolib v1.5");
		ImGui::Text("PCG-c-0.94");
		ImGui::Text("ImGui 1.65");
		ImGui::Text("SDL");
		ImGui::Text("STL");
		///---------------------------------
		ImGui::Separator();
		ImGui::TextWrapped("MIT License Copyright(c) 2018 Oscar Hernandez and Aleix Gabarro	Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the Software), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:");
		ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
		ImGui::TextWrapped("THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

		ImGui::End();
	}

	if (configurationWindow)
	{
		ImGui::Begin("Configuration", &configurationWindow);

		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::SliderInt("Max Fps", &App->capFrames, 30, 144);

			ImGui::Checkbox("Cap Fps", &App->toCap);

			char graphTitle[25];

			sprintf_s(graphTitle, 25, "Framerate %.1f", App->fpsLog[App->fpsLog.size() - 1]);
			ImGui::PlotHistogram("##Framerate", &App->fpsLog[0], App->fpsLog.size(), 0, graphTitle, 0.0f, 150.0f, ImVec2(310, 100));

			sprintf_s(graphTitle, 25, "Milliseconds %.1f", App->msLog[App->msLog.size() - 1]);
			ImGui::PlotHistogram("##Framerate", &App->msLog[0], App->msLog.size(), 0, graphTitle, 0.0f, 40.0f, ImVec2(310, 100));
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
			{
				App->window->SetFullscreen();
			}
			ImGui::SameLine();
			if (ImGui::Checkbox("Borderless", &App->window->borderless))
			{
				App->window->SetBorderless();
			}
			if (ImGui::SliderFloat("Brightness", &brightnessPos, 0.0f, 1.0f))
			{
				SDL_SetWindowBrightness(App->window->window, brightnessPos);
			}
			if (ImGui::SliderInt("Width", &widthPos, 640, 1920))
			{
				SDL_SetWindowSize(App->window->window, widthPos, heightPos);
			}
			if (ImGui::SliderInt("Height", &heightPos, 480, 1080))
			{
				SDL_SetWindowSize(App->window->window, widthPos, heightPos);
			}

		}

		if (ImGui::CollapsingHeader("Hardware"))
		{	
			//CPU--------------------------------------------------------
			ImVec4 color(1.0f, 1.0f, 0.1f, 1.0f);

			ImGui::Text("CPUs: "); ImGui::SameLine();
			ImGui::TextColored(color, "%i (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

			ImGui::Text("System RAM: "); ImGui::SameLine();
			ImGui::TextColored(color, "%.2fGB", (float)SDL_GetSystemRAM() / 1024);


			ImGui::Text("Caps: "); ImGui::SameLine();
			string caps("");
			CheckCaps(&caps);

			ImGui::TextColored(color, caps.data());

			ImGui::Separator();

			//GPU--------------------------------------------------------
			const GLubyte* gpuInfo = glGetString(GL_VENDOR);
			const GLubyte* renderer = glGetString(GL_RENDERER);

			ImGui::Text("GPU info:"); ImGui::SameLine();
			ImGui::TextColored(color,(char*)gpuInfo); ImGui::SameLine();
			ImGui::TextColored(color,(char*)renderer);


			GLint nTotalMemoryInKB = 0;
			glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
				&nTotalMemoryInKB);

			GLint nCurAvailMemoryInKB = 0;
			glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
				&nCurAvailMemoryInKB);

			ImGui::Text("VRAM total: "); ImGui::SameLine();
			ImGui::TextColored(color, "%.2f MB", (float)nTotalMemoryInKB / 1024);

			ImGui::Text("VRAM available: "); ImGui::SameLine();
			ImGui::TextColored(color, "%.2f MB", (float)nCurAvailMemoryInKB / 1024);

			ImGui::Text("VRAM in use: "); ImGui::SameLine();
			ImGui::TextColored(color, "%.2f MB", ((float)nTotalMemoryInKB / 1024) - (float)nCurAvailMemoryInKB / 1024);
		}
		ImGui::End();
	}

	if (consoleWindow)
	{
		/*ImGui::Begin("Console", &consoleWindow);
		
		
		ImGui::End();
		ImGui::ShowExampleAppConsole(&consoleWindow);
		ImGui::*/
		
	}
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::MenuItem("Example window", "Ctrl+E", exampleWindow))
				exampleWindow = !exampleWindow;

			else if (ImGui::MenuItem("Demo window", "Ctrl+D", showDemoWindow))
				showDemoWindow = !showDemoWindow;

			else if (ImGui::MenuItem("Math window", "Ctrl+M", mathGeoLibWindow))
				mathGeoLibWindow = !mathGeoLibWindow;

			else if (ImGui::MenuItem("Random number window", "Ctrl+R", randomNumberWindow))
				randomNumberWindow = !randomNumberWindow;

			else if (ImGui::MenuItem("About", "Ctrl+A", aboutWindow))
				aboutWindow = !aboutWindow;

			if (ImGui::MenuItem("Configuration", "Ctrl+C", configurationWindow))
				configurationWindow = !configurationWindow;

			if (ImGui::MenuItem("Console", "Ctrl+GRAVE"))
				consoleWindow = !consoleWindow;

			ImGui::EndMenu();
		}

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
				exampleWindow = !exampleWindow;

			else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
				showDemoWindow = !showDemoWindow;

			else if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
				mathGeoLibWindow = !mathGeoLibWindow;

			else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
				randomNumberWindow = !randomNumberWindow;

			else if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
				aboutWindow = !aboutWindow;

			else if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
				configurationWindow = !configurationWindow;

			else if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
				consoleWindow = !consoleWindow;

		}

		if (ImGui::BeginMenu("Exit"))
		{
			return update_status::UPDATE_STOP;
			//ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	created = true;


	return UPDATE_CONTINUE;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
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

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}




void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

void ModuleSceneIntro::CheckCaps(string* caps)
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
