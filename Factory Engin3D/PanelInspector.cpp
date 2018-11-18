#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"
#include "Texture.h"

void ModuleImGui::CreateInspector(float2 scale)
{
	ImGui::Begin("Inspector", &inspectorWindow);

	SetWindowDim(inspectorPos, inspectorSize, scale);

	GameObject* currObject = App->geometry->currentGameObject;
	if (currObject != nullptr)
	{
		std::string goName = currObject->name;
		ImGui::Text(goName.data());

		if (ImGui::Checkbox("Active", currObject->GetActiveReference()))
		{
			currObject->SetActive(currObject->GetActive());
			if (currObject->GetActive())
				App->sceneIntro->ReInsertOctree(currObject);
			App->gameObject->redoOc = true;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Static Object", currObject->GetStaticReference()))
		{
			currObject->SetObjectStatic(currObject->GetObjectStatic());
			if (!currObject->GetObjectStatic())
				App->gameObject->AddNewDynamic(currObject);
			else
				App->gameObject->RemoveDynamic(currObject);

			App->gameObject->redoOc = true;
		}

		for (std::list<Component*>::iterator comp = currObject->components.begin(); comp != currObject->components.end(); ++comp)
		{
			(*comp)->Inspector();
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Add new component"))
		{
			
			if (ImGui::MenuItem("Geometry", "", nullptr, !currObject->HasComponent(ComponentType_GEOMETRY)))
			{
				GeometryInfo info;
				info.geometry = new Mesh();

				currObject->AddComponent(ComponentType_GEOMETRY, &info);
			}
			
			if (ImGui::MenuItem("Texture", "", nullptr, !currObject->HasComponent(ComponentType_TEXTURE)))
			{
				TextureInfo info;
				info.texture = nullptr;
				currObject->AddComponent(ComponentType_TEXTURE, &info);
			}

			if (ImGui::MenuItem("Camera", "", nullptr, !currObject->HasComponent(ComponentType_CAMERA)))
			{
				currObject->AddComponent(ComponentType_CAMERA, nullptr);
			}

			ImGui::MenuItem("Cancel");

			ImGui::EndMenu();
		}


		ImGui::Separator();
		if (ImGui::Button("Delete", ImVec2(100, 20)))
			DeleteGO(currObject);
	}
	else 
	{
		ImGui::TextWrapped("Select a GameObject to view its components");
	}
	ImGui::End();
}

void ModuleImGui::DeleteGO(GameObject *& object)
{
	if (App->time->gameState == GameState_NONE)
		object->Delete();
	else
	{
		object->toDeleteFake = object->GetActive();
		object->SetActive(false);
	}
	App->geometry->currentGameObject = nullptr;
	App->gameObject->redoOc = true;
}