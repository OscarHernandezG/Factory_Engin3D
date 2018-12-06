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
			//gameObject->GetGlobalMatrix().Float3x3Part() * float3(0,1,0);
			yAxis = camera->frustum.up;

			xAxis = yAxis.Cross(zAxis).Normalized();

			break;
		case Billboard_WORLD:

			zAxis = float3(camera->GetPos() - obPos).Normalized();
			yAxis = camera->frustum.up;
			xAxis = yAxis.Cross(zAxis).Normalized();

			yAxis = zAxis.Cross(xAxis).Normalized();

			break;
		case Billboard_AXIALY:

			if (xAxisRot)
			{
				zAxis = float3(camera->GetPos() - obPos).Normalized();
				xAxis = gameObject->GetGlobalRotation() * float3(1, 0, 0);
				yAxis = zAxis.Cross(xAxis).Normalized();

				zAxis = xAxis.Cross(yAxis).Normalized();

			}
			else if (yAxisRot)
			{
				zAxis = float3(camera->GetPos() - obPos).Normalized();
				yAxis = gameObject->GetGlobalRotation() * float3(0, 1, 0);
				xAxis = yAxis.Cross(zAxis).Normalized();

				zAxis = xAxis.Cross(yAxis).Normalized();

			}

			else if (zAxisRot)
			{
				xAxis = float3(camera->GetPos() - obPos).Normalized();
				zAxis = gameObject->GetGlobalRotation() * float3(0, 0, 1);
				yAxis = zAxis.Cross(xAxis).Normalized();

				xAxis = yAxis.Cross(zAxis).Normalized();
			}
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

		if (typeBoard == Billboard_AXIALY)
		{
			if (ImGui::RadioButton("X", xAxisRot))
			{
				xAxisRot = true;
				yAxisRot = false;
				zAxisRot = false;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Y", yAxisRot))
			{
				xAxisRot = false;
				yAxisRot = true;
				zAxisRot = false;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Z", zAxisRot))
			{
				xAxisRot = false;
				yAxisRot = false;
				zAxisRot = true;
			}
		}
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

			else if (ImGui::MenuItem("Axialy"))
			{
				typeBoard = Billboard_AXIALY;
				currentType = "Axialy Billboard";
			}

			else if (ImGui::MenuItem("NONE"))
			{
				typeBoard = Billboard_NONE;
				currentType = "NONE";
			}

			ImGui::End();
		}
		if (ImGui::Button("Remove Billboard", ImVec2(150, 25)))
			toDelete = true;
	}
}
