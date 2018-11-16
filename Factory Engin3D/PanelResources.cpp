#include "Application.h"
#include "ModuleImGui.h"
#include "imgui-1.65/imgui_dock.h"

void ModuleImGui::CreateAssetsWindow(float2 scale)
{
	ImGui::BeginDock("Assets");

		//SetWindowDim(configurationPos, configurationSize, scale);

		if (contRefresh.ReadSec() >= 1)
		{
			RefreshAssets(".\\Assets\\");
			contRefresh.Start();
		}
	DrawAssets(assetsHierarchy);

	if (popUp)
	{
		ImGui::OpenPopup("FilePopup");
		if (ImGui::BeginPopup("FilePopup"))
		{
			if (ImGui::MenuItem("Load"))
			{
				App->importer->DistributeFile(pathClicked.data(), true);
				popUp = false;
			}
			if (ImGui::MenuItem("Close"))
				popUp = false;
			ImGui::EndPopup();
		}
		if ((ImGui::IsMouseClicked(0) || ImGui::IsMouseClicked(1)) && !ImGui::IsAnyItemHovered())
			popUp = false;
	}

	ImGui::EndDock();
}

void ModuleImGui::DrawAssets(AssetsHierarchy& assets)
{
	for (std::vector<AssetsHierarchy>::iterator iter = assets.childFiles.begin(); iter != assets.childFiles.end(); ++iter)
	{
		ImGuiTreeNodeFlags_ flag = ImGuiTreeNodeFlags_None;
		if ((*iter).childFiles.empty())
			flag = ImGuiTreeNodeFlags_Leaf;

		if (ImGui::TreeNodeEx((*iter).file.data(), flag))
		{
			DrawAssets(*iter);
			ImGui::TreePop();
		}

		if (ImGui::IsItemClicked(1))
		{
			popUp = true;
			pathClicked = (*iter).file.data();
		}
	}
}

void ModuleImGui::RefreshAssets(const char* path)
{
	assetsHierarchy.file = "Assets";
	App->resources->ReadFolder(path, assetsHierarchy.childFiles);
}