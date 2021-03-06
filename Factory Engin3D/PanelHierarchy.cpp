#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateGameObjectHierarchy(float2 scale)
{
	if (ImGui::Begin("Scene", &hierarchyWindow, ImGuiWindowFlags_NoMove))


	//Options menu
	if (ImGui::BeginMenu("Options"))
	{
		if (ImGui::MenuItem("New Game Object"))
		{
			std::string str = "Random Object ";
			str.append(std::to_string(rand() % 100));
			App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->rootGameObject, str.data());
		}

		ImGui::MenuItem("Close");
		ImGui::EndMenu();
	}
	ImGui::Separator();

	//Iterate all GO in sceene
	if (App->gameObject->rootGameObject)
	{
		CreateGOTreeNode(App->gameObject->rootGameObject);
		if (objectDrag)
			objectDrag = false; //put false for next frame
	}
	//Call only one time OpenPopup
	if (popHierarchy)
	{
		ImGui::OpenPopup("GameObjectsPop");
		popHierarchy = false;
	}
	//Opened every frame if popup its opened
	if (ImGui::BeginPopup("GameObjectsPop"))
	{
		std::string str = "New Game Object in ";
		str.append(objectSelected->name);
		if (ImGui::MenuItem(str.data()))
		{
			str = "Random Object ";
			str.append(std::to_string(rand() % 100));
			App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, objectSelected, str.data());
		}
		if (ImGui::MenuItem("Delete"))
			objectSelected->Delete();
		ImGui::EndPopup();
	}
	else if (objectSelected)
	{
		objectSelected = nullptr;
	}

	ImVec2 size = ImGui::GetWindowSize();
	sceneSize.x = size.x;

	SetWindowDim(scenePos, sceneSize, scale);
	ImGui::End();
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
		DragDropGO(current);
		if (ImGui::IsItemClicked(0))
		{
			App->geometry->currentGameObject = current;
		}

		for (std::list<GameObject*>::iterator childs = current->childs.begin(); childs != current->childs.end(); ++childs)
		{
			CreateGOTreeNode(*childs);

			if (ImGui::IsItemClicked(1) && objectSelected == nullptr)
			{
				objectSelected = *childs;
				popHierarchy = true;
			}
		}
		ImGui::TreePop();
	}
	else
		DragDropGO(current);
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
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Change GO hierarchy");
		if (payload)
		{
			GameObject* childObject = *(GameObject**)payload->Data;
				
			if (childObject &&  childObject != App->gameObject->rootGameObject &&!objectDrag)
			{
				objectDrag = true; //Enter one time, only the child

				float4x4 global = childObject->GetGlobalMatrix();
				float4x4 parentGlobal = object->GetGlobalMatrix();

				parentGlobal.Inverse();

				float4x4 local = parentGlobal.Mul(global);

				childObject->SetTransform(local);


				childObject->SetParent(object);
			}
		}
		ImGui::EndDragDropTarget();
	}
}