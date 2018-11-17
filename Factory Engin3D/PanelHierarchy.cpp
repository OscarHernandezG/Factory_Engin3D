#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateGameObjectHierarchy(float2 scale)
{

	ImGui::Begin("Scene", &hierarchyWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(scenePos, sceneSize, scale);

	if (App->gameObject->rootGameObject)
	{
		CreateGOTreeNode(App->gameObject->rootGameObject);
		if (objectDrag)
			objectDrag = false; //put false for next frame
	}
	ImGui::End();

	if (ImGui::IsMouseClicked(1) && !popHierarchy)
		ImGui::OpenPopup("GameObjectsPop");
	
	popHierarchy = ImGui::BeginPopup("GameObjectsPop");
	if (popHierarchy)
	{
		if (ImGui::MenuItem("New Game Object"))
		{
			std::string str = std::to_string((rand() % 100));
			if(objectSelected)
				App->gameObject->CreateEmptyGameObject(objectSelected, str.data());
			else
				App->gameObject->CreateEmptyGameObject(App->gameObject->rootGameObject, str.data());
		}
		if(objectSelected)
			if (ImGui::MenuItem("Delete"))
				objectSelected->Delete();
			

		ImGui::MenuItem("Close");
		ImGui::EndPopup();
	}
	else if(objectSelected)
	{
		objectSelected = nullptr;
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

			DragDropGO(*childs);

			if(ImGui::IsItemClicked(1))
				objectSelected = *childs;
		}
		ImGui::TreePop();
	}
}

void ModuleImGui::DragDropGO(GameObject* &object)
{
	if (!objectDrag)
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptBeforeDelivery))
		{
			objectDrag = true; //Enter one time, only the child
			ImGui::SetDragDropPayload("Change GO hierarchy", &object, sizeof(GameObject*));
			ImGui::Text("Move %s", object->name.data());
			ImGui::EndDragDropSource();
		}
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Change GO hierarchy"))
		{
			GameObject* childObject = *(GameObject**)payload->Data;
			if (childObject && !objectDrag)
			{
				objectDrag = true; //Enter one time, only the child
				childObject->SetParent(object);
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void ModuleImGui::DeleteGO(GameObject* &object)
{
	if (App->time->gameState == GameState_NONE)
		object->Delete();
	else
	{
		object->toDeleteFake = object->GetActive();
		object->SetActive(false);
	}

	App->sceneIntro->redoOc = true;
}