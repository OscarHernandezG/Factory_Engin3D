#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateAssetsWindow(float2 scale)
{
	ImGui::Begin("Assets", &canScroll, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	SetWindowDim(configurationPos, configurationSize, scale);

	TreeAssets(".\\Assets\\");

	ImGui::End();
}

void ModuleImGui::TreeAssets(const char* path)
{
	std::vector<std::string> filesStr = App->resources->ReadFolder(path);

	for (std::vector<std::string>::iterator iter = filesStr.begin(); iter != filesStr.end(); ++iter)
	{
		ImGuiTreeNodeFlags_ flag = ImGuiTreeNodeFlags_None;
		std::string newPath = path;
		if (App->resources->ExistFile(newPath.append(*iter).data()))
			flag = ImGuiTreeNodeFlags_Leaf;
		if (ImGui::TreeNodeEx((*iter).c_str(), flag))
		{
			newPath += "\\";
			TreeAssets(newPath.data());
			ImGui::TreePop();
		}
	}
}
