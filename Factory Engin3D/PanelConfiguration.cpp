#include "Application.h"
#include "ModuleImGui.h"
#include "SDL/include/SDL_opengl.h"

void ModuleImGui::CreateConfigWindow(float2 scale)
{
	ImGui::Begin("Configuration", &configurationWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(configurationPos, configurationSize, scale);

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

//Create Headers----------------------------------------------------------
void ModuleImGui::CreateAppHeader()
{

	static char appName[64];
	sprintf_s(appName, 64, App->aplicationName.data());
	if (ImGui::InputText("Aplication Name", appName, 64, ImGuiInputTextFlags_EnterReturnsTrue))
		App->ChangeAppName(appName);

	ImGui::SliderInt("Max Fps", &App->capFrames, 30, 144);

	ImGui::Checkbox("Cap Fps", &App->toCap);

	char graphTitle[25];

	sprintf_s(graphTitle, 25, "Framerate %.1f", App->time->fpsLog[App->time->fpsLog.size() - 1]);
	ImGui::PlotHistogram("##Framerate", &App->time->fpsLog[0], App->time->fpsLog.size(), 0, graphTitle, 0.0f, 150.0f, ImVec2(310, 100));

	sprintf_s(graphTitle, 25, "Milliseconds %.1f", App->time->msLog[App->time->msLog.size() - 1]);
	ImGui::PlotHistogram("##Milliseconds", &App->time->msLog[0], App->time->msLog.size(), 0, graphTitle, 0.0f, 40.0f, ImVec2(310, 100));

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
		App->window->fullscreen = false;
		App->window->SetFullscreen(true);
		//App->window->fulldesktop = false;
		//warningDialoge = true;
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
	if (App->geometry->currentGameObject != nullptr)
	{
		Geometry* goGeometry = (Geometry*)App->geometry->currentGameObject->GetComponent(ComponentType::ComponentType_GEOMETRY);
		if (goGeometry != nullptr)
		{
			if (goGeometry->GetType() == PrimitiveTypes::Primitive_Mesh)
			{

				Mesh* mesh = (Mesh*)goGeometry;

				ImGui::Text("Total vertex: %i", mesh->buffer->vertex.size);
				ImGui::Text("Total faces: %i", App->geometry->numFaces);
				if (ImGui::Button("Remove Mesh", { 125,25 }))
				{
					Geometry* mesh = (Geometry*)App->geometry->currentGameObject->GetComponent(ComponentType::ComponentType_GEOMETRY);
					App->geometry->currentGameObject->RemoveComponent(mesh);
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
	//if (App->geometry->textureID != 0)
	//{
	//	Geometry* currentGeometry = (Geometry*)App->geometry->currentGameObject->GetComponent(ComponentType_GEOMETRY);
	//	if (currentGeometry != nullptr)
	//		if (currentGeometry->GetType() == PrimitiveTypes::Primitive_Mesh)
	//		{
	//			Mesh* mesh = (Mesh*)currentGeometry;

	//			ImGui::Text("Texture id: %i", App->geometry->textureID);
	//			// TODO
	//			//ImGui::Text("Texture used by %i meshes", mesh->buffers.size());
	//			ImGui::Text("UV Preview");
	//			ImGui::Separator();
	//			ImGui::Image((void*)App->geometry->textureID, { 200,200 });
	//			if (ImGui::Button("Remove Texture", { 125,25 }))
	//			{
	//				glDeleteTextures(1, &App->geometry->textureID);
	//				App->geometry->textureID = 0;
	//			}
	//		}
	//}
	//else
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
	std::string caps("");
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

void ModuleImGui::CheckCaps(std::string* caps)
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
