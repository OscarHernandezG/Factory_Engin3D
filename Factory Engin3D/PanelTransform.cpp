#include "Globals.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleWindow.h"
#include "ModuleResources.h"
void ModuleImGui::CreateTransform(float2 scale)
{
	ImGui::Begin("Transform", &transformWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(transformPos, transformSize, scale);

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
			App->sceneIntro->octree.ReDoOctree(AABB(), true);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Static Object", currObject->GetStaticReference()))
		{
			currObject->SetObjectStatic(currObject->GetObjectStatic());
			if (!currObject->GetObjectStatic())
				App->gameObject->AddNewDynamic(currObject);
			else
				App->gameObject->RemoveDynamic(currObject);

			App->sceneIntro->octree.ReDoOctree(AABB(), true);
		}
		float3 position, scale, angles;
		Quat rotate;

		if (currObject->transform != nullptr)
		{
			float4x4 prevTransformMat = currObject->GetGlobalMatrix();
			// Use go, not trans!
			position = currObject->transform->GetPos();
			scale = currObject->transform->GetScale();
			rotate = currObject->transform->GetRotation();

			if (ImGui::DragFloat3("Position", &position[0]) && App->gameObject->CanTransform(currObject)) {
				if (dragTransform)
				{
					App->sceneIntro->SaveLastTransform(prevTransformMat);
					dragTransform = false;
				}
				currObject->SetPos(position);
				App->sceneIntro->octree.ReDoOctree(AABB(), true);
			}

			if (ImGui::DragFloat3("Scale", &scale[0]) && App->gameObject->CanTransform(currObject))
			{
				if (dragTransform)
				{
					App->sceneIntro->SaveLastTransform(prevTransformMat);
					dragTransform = false;
				}
				currObject->SetScale(scale);
				App->sceneIntro->octree.ReDoOctree(AABB(), true);
			}
			angles = rotate.ToEulerXYZ();

			angles[0] = math::RadToDeg(angles.x);
			angles[1] = math::RadToDeg(angles.y);
			angles[2] = math::RadToDeg(angles.z);

			if (ImGui::DragFloat3("Rotation", &angles[0]) && App->gameObject->CanTransform(currObject))
			{
				if (dragTransform)
				{
					App->sceneIntro->SaveLastTransform(prevTransformMat);
					dragTransform = false;
				}
				angles.x = math::DegToRad(angles.x);
				angles.y = math::DegToRad(angles.y);
				angles.z = math::DegToRad(angles.z);
				currObject->SetRotation(Quat::FromEulerXYZ(angles.x, angles.y, angles.z));

				App->sceneIntro->octree.ReDoOctree(AABB(), true);
			}
			else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
				dragTransform = true;

			if (ImGui::Button("Reset", ImVec2(100, 20)))
			{
				App->sceneIntro->SaveLastTransform(prevTransformMat);
				currObject->SetIdentity();
				App->sceneIntro->octree.ReDoOctree(AABB(), true);
			}

			if (ImGui::RadioButton("None", App->sceneIntro->GetGuizOperation() == ImGuizmo::BOUNDS))
				App->sceneIntro->SetGuizOperation(ImGuizmo::BOUNDS);
			ImGui::SameLine();
			if (ImGui::RadioButton("Move", App->sceneIntro->GetGuizOperation() == ImGuizmo::TRANSLATE))
				App->sceneIntro->SetGuizOperation(ImGuizmo::TRANSLATE);
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", App->sceneIntro->GetGuizOperation() == ImGuizmo::SCALE))
				App->sceneIntro->SetGuizOperation(ImGuizmo::SCALE);
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", App->sceneIntro->GetGuizOperation() == ImGuizmo::ROTATE))
				App->sceneIntro->SetGuizOperation(ImGuizmo::ROTATE);
		}
	}
	else {
		ImGui::TextWrapped("There aren't any meshes");
	}
	ImGui::End();
}
