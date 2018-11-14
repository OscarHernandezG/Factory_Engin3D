#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateAboutWindow(float2 scale)
{
	ImGui::Begin("About", &aboutWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(aboutPos, aboutSize, scale);

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
	std::string nameStr = "ImGui ";
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

	sprintf_s(nameChar, 25, "Guizmos");
	if (ImGui::Button(nameChar, ImVec2(125, 20)))
		ShellExecuteA(NULL, "Open", "https://github.com/CedricGuillemet/ImGuizmo", NULL, NULL, SW_SHOWNORMAL);
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

