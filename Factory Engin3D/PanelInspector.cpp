#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"
#include "ComponentTexture.h"

void ModuleImGui::CreateInspector(float2 scale)
{
	ImGui::Begin("Inspector", &inspectorWindow,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysHorizontalScrollbar);


	GameObject* currObject = App->geometry->currentGameObject;
	if (currObject != nullptr)
	{
		char name[50];
		strcpy_s(name, 50, currObject->name.c_str());
		if (ImGui::InputText("##Name", name, 50, ImGuiInputTextFlags_EnterReturnsTrue))
			currObject->name = name;

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

			if (ImGui::MenuItem("Billboard", "", nullptr, !currObject->HasComponent(ComponentType_BILLBOARD)))
			{
				currObject->AddComponent(ComponentType_BILLBOARD, nullptr);
			}	
			
			if (ImGui::MenuItem("Particle System", "", nullptr, !currObject->HasComponent(ComponentType_EMITTER)))
			{
				currObject->AddComponent(ComponentType_EMITTER, nullptr);
			}
			ImGui::MenuItem("Cancel");

			ImGui::EndMenu();
		}


		ImGui::Separator();
		if (ImGui::Button("Delete GameObject", ImVec2(150, 25)))
		{
			currObject->Delete();
			App->geometry->currentGameObject = nullptr;
			App->gameObject->redoOc = true;
		}
	}
	else 
	{
		ImGui::TextWrapped("Select a GameObject to view its components");
	}

	ImVec2 size = ImGui::GetWindowSize();
	inspectorSize.x = size.x;

	SetWindowDim(inspectorPos, inspectorSize, scale);

	ImGui::End();
}
