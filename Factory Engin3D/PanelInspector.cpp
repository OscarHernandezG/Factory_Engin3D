#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"

void ModuleImGui::CreateInspector(float2 scale)
{
	ImGui::Begin("Inspector", &inspectorWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

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
			App->sceneIntro->redoOc = true;
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Static Object", currObject->GetStaticReference()))
		{
			currObject->SetObjectStatic(currObject->GetObjectStatic());
			if (!currObject->GetObjectStatic())
				App->gameObject->AddNewDynamic(currObject);
			else
				App->gameObject->RemoveDynamic(currObject);

			App->sceneIntro->redoOc = true;
		}

		for (std::list<Component*>::iterator comp = currObject->components.begin(); comp != currObject->components.end(); ++comp)
		{
			(*comp)->Inspector();
		}

		if (ImGui::Button("Delete", ImVec2(100, 20)))
		{
			DeleteGO(currObject);
			App->geometry->currentGameObject = nullptr;
			App->sceneIntro->redoOc = true;
		}
	}
	else {
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

	App->sceneIntro->redoOc = true;

}