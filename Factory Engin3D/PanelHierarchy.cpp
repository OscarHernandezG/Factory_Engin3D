#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateGameObjectHierarchy(float2 scale)
{

	ImGui::Begin("Scene", &hierarchyWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(scenePos, sceneSize, scale);

	if (App->gameObject->rootGameObject)
	{
		CreateGOTreeNode(App->gameObject->rootGameObject);
	}
	ImGui::End();

	if (ImGui::IsMouseClicked(1) && !popHierarchy)
		ImGui::OpenPopup("GameObjectsPop");
	
	if (popHierarchy = ImGui::BeginPopup("GameObjectsPop"))
	{
		if (ImGui::MenuItem("GO"))
		{
			if(parentObject)
				App->gameObject->CreateEmptyGameObject(parentObject, "Empty Object");
			else
				App->gameObject->CreateEmptyGameObject(App->gameObject->rootGameObject, "Empty Object");
		}
		ImGui::MenuItem("Close");
		ImGui::EndPopup();
	}
	else if(parentObject)
	{
		parentObject = nullptr;
	}
}

void ModuleImGui::CreateGOTreeNode(GameObject* current)
{
	uint flags = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;// ImGuiTreeNodeFlags_OpenOnArrow;
	if (current->childs.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (current == App->geometry->currentGameObject)
		flags |= ImGuiTreeNodeFlags_Selected;

	char name[80];

	std::string sName = current->name;

	sprintf_s(name, 80, "%s", sName.c_str());


	if (ImGui::TreeNodeEx(name, flags))
	{
		if (ImGui::IsItemClicked(0))
		{
			App->geometry->currentGameObject = current;
		}

		for (std::list<GameObject*>::iterator childs = current->childs.begin(); childs != current->childs.end(); ++childs)
		{
			CreateGOTreeNode(*childs);
			if(ImGui::IsItemClicked(1))
				parentObject = *childs;
		}
		ImGui::TreePop();
	}
}