#include "Application.h"

#include "Billboard.h"
 

Billboard::Billboard(GameObject* gameObject) : Component(gameObject)
{

}

Billboard::~Billboard()
{

}

void Billboard::Update() {

	if (gameObject->father && typeBoard != Billboard_NONE)
	{
		float3 xAxis, yAxis, zAxis;
		float3 obPos = gameObject->GetGlobalPos();
		Camera* camera = App->geometry->GetPlayingCamera();
		
		switch (typeBoard)
		{
		case Billboard_NONE:
			break;
		case Billboard_SCREEN:

			zAxis = -camera->frustum.front;
			yAxis = camera->frustum.up;

			xAxis = zAxis.Cross(yAxis).Normalized();

			break;
		case Billboard_WORLD:

			zAxis = float3(camera->GetPos() - obPos).Normalized();
			yAxis = camera->frustum.up;
			xAxis = yAxis.Cross(zAxis).Normalized();

			yAxis = zAxis.Cross(xAxis).Normalized();

			break;
		case Billboard_AXIS:
			break;
		default:
			break;
		}

		RayLine ray(obPos, xAxis * 10);
		ray.InnerRender();

		RayLine ray2(obPos, yAxis * 10);
		ray2.InnerRender();

		RayLine ray3(obPos, zAxis * 10);
		ray3.InnerRender();

		float3x3 rot = float3x3(xAxis, yAxis, zAxis);

		float4x4 newOrient = float4x4::FromTRS(gameObject->GetGlobalPos(), rot, gameObject->GetGlobalScale());


		float4x4 temp = gameObject->father->GetGlobalMatrix();
		temp.Inverse();

		newOrient = temp.Mul(newOrient);
		if (gameObject->transform)
			gameObject->transform->SetTransform(newOrient);

	}
}

void Billboard::Inspector()
{
	if (ImGui::CollapsingHeader("Billboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Current bilboard:");
		ImGui::SameLine();
		ImGui::Text(currentType.data());

		if (ImGui::BeginMenu("Change your billboard"))
		{
			if (ImGui::MenuItem("Screen"))
			{
				typeBoard = Billboard_SCREEN;
				currentType = "Screen Billboard";
			}
			else if (ImGui::MenuItem("World"))
			{
				typeBoard = Billboard_WORLD;
				currentType = "World Billboard";
			}

			else if (ImGui::MenuItem("Axis"))
			{
				typeBoard = Billboard_AXIS;
				currentType = "Axis Billboard";
			}

			else if (ImGui::MenuItem("NONE"))
			{
				typeBoard = Billboard_NONE;
				currentType = "NONE";
			}

			ImGui::End();
		}
	}
}
