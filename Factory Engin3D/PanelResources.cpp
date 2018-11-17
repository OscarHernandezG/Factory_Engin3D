#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateAssetsWindow(float2 scale)
{
	ImGui::Begin("Assets", &canScroll, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	SetWindowDim(configurationPos, configurationSize, scale);

	if (contRefresh.ReadSec() >= 1)
	{
		RefreshAssets(".\\Assets\\");
		contRefresh.Start();
	}
	DrawAssets(assetsHierarchy);

	ImGui::End();
}

void ModuleImGui::DrawAssets(AssetsHierarchy& assets)
{
	for (std::vector<AssetsHierarchy>::iterator iter = assets.childFiles.begin(); iter != assets.childFiles.end(); ++iter)
	{
		ImGuiTreeNodeFlags_ flag = ImGuiTreeNodeFlags_None;
		if ((*iter).childFiles.empty())
			flag = ImGuiTreeNodeFlags_Leaf;

		if (ImGui::TreeNodeEx((*iter).file.data(),flag))
		{
			DrawAssets(*iter);
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked(App->input->GetMouseButton(SDL_BUTTON_RIGHT)))
		{
			App->importer->DistributeFile((char*)((*iter).file.data()),true);
		}
	}
}

void ModuleImGui::RefreshAssets(const char* path)
{
	assetsHierarchy.file = "Assets";
	App->resources->ReadFolder(path, assetsHierarchy.childFiles);
}