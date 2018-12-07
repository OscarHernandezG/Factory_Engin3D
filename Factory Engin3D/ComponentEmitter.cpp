#include "Application.h"

#include "ComponentEmitter.h"
#include "Application.h"
#include "ModuleTime.h"

#include <vector>

ComponentEmitter::ComponentEmitter(GameObject* gameObject) : Component(gameObject, ComponentType_EMITTER)
{
	timer.Start();
}


ComponentEmitter::~ComponentEmitter()
{
}

void ComponentEmitter::Update()
{

	if (timer.ReadSec() >  1.0f / rateOverTime && (App->particle->particleList.size() < MAX_PARTICLES) && (loop || loopTimer.ReadSec() < duration) )
	{
		float3 pos = RandPos();
		Particle* newParticle = new Particle(pos, startValues, &texture);

		particles.push_back(newParticle);
		App->particle->particleList.push_back(newParticle);
		timer.Start();
	}

	std::vector<Particle*> particleDelete;
	for (std::list<Particle*>::iterator iterator = particles.begin(); iterator != particles.end(); ++iterator)
	{
		if (!(*iterator)->Update(App->time->GetdtGame()) || toDelete)
			particleDelete.push_back(*iterator);
	}

	for (std::vector<Particle*>::iterator iterator = particleDelete.begin(); iterator != particleDelete.end(); ++iterator)
	{
		particles.remove(*iterator);
		App->particle->particleList.remove(*iterator);

		delete *iterator;
	}

}

float3 ComponentEmitter::RandPos()
{
	float3 spawn = float3::zero;
	switch (shapeType)
	{
	case ShapeType_BOX:
		spawn = boxCreation.RandomPointInside(App->randomMath);
		break;

	case ShapeType_SPHERE:
		spawn = SphereCreation.RandomPointInside(App->randomMath);
		startValues.particleDirection = spawn.Normalized();
		break;

	case ShapeType_SPHERE_CENTER:
		startValues.particleDirection = SphereCreation.RandomPointInside(App->randomMath).Normalized();
		break;

	case ShapeType_SPHERE_BORDER:
		spawn = SphereCreation.RandomPointOnSurface(App->randomMath);
		startValues.particleDirection = spawn.Normalized();
		break;

	default:
		break;
	}

	float3 global = float3::zero;
	if (gameObject)
		global = gameObject->GetGlobalPos();

	return spawn + global;
}

void ComponentEmitter::Inspector()
{
	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushItemWidth(150.0f);
		ImGui::DragFloat("Speed", &startValues.speed, 0.25f, 0.25f, 20.0f, "%.2f");
		ImGui::DragFloat("Acceleration", &startValues.acceleration, 0.25f, -5.0f, 5.0f, "%.2f");

		ImGui::DragFloat("Rotation", &startValues.rotation, 0.25f, -720.0f, 720.0f, "%.2f");
		ImGui::DragFloat("Angular acceleration", &startValues.angularAcceleration, 0.25f, -45.0f, 45.0f, "%.2f");

		ImGui::DragInt("Emition", &rateOverTime, 1.0f, 1, 50, "%.2f");
		ImGui::DragFloat("Lifetime", &startValues.life, 0.5f, 1.0f, 20.0f, "%.2f");
		ImGui::DragFloat("Size", &startValues.size, 0.1f, 0.1f, 5.0f, "%.2f");

		ImGui::Separator();
		if (ImGui::Checkbox("Loop", &loop))
			loopTimer.Start();
		ImGui::DragFloat("Duration", &duration, 0.5f, 0.5f, 20.0f, "%.2f");

		ImGui::Separator();
		if (ImGui::BeginMenu("Change Shape"))
		{
			if (ImGui::MenuItem("Box"))
			{
				shapeType = ShapeType_BOX;
				startValues.particleDirection = float3::unitY;
			}
			else if (ImGui::MenuItem("Sphere"))
				shapeType = ShapeType_SPHERE;
			ImGui::End();
		}


		float3 pos;
		switch (shapeType)
		{
		case ShapeType_BOX:
			ImGui::Text("Box");
			pos = boxCreation.Size();
			ImGui::DragFloat3("Box Size", &pos.x, 0.1f, 0.1f, 20.0f, "%.2f");

			boxCreation.SetFromCenterAndSize(boxCreation.CenterPoint(), pos);

			break;
		case ShapeType_SPHERE:
		case ShapeType_SPHERE_BORDER:
		case ShapeType_SPHERE_CENTER:
			ImGui::Text("Sphere");

			ImGui::Text("Particle emision from:");

			if (ImGui::RadioButton("Random", shapeType == ShapeType_SPHERE))
				shapeType = ShapeType_SPHERE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Center", shapeType == ShapeType_SPHERE_CENTER))
				shapeType = ShapeType_SPHERE_CENTER;
			ImGui::SameLine();
			if (ImGui::RadioButton("Border", shapeType == ShapeType_SPHERE_BORDER))
				shapeType = ShapeType_SPHERE_BORDER;

			ImGui::DragFloat("Sphere Size", &SphereCreation.r, 0.25f, 1.0f, 20.0f, "%.2f");

			break;
		default:
			break;
		}

		ImGui::Separator();
		ImGui::PopItemWidth();
		ImGui::Text("Particle Color");
		for (std::list<ColorTime>::iterator iter = startValues.color.begin(); iter != startValues.color.end(); ++iter)
		{
			//TODO: they must be able to change color 
			EditColor(*iter);
			if (!startValues.timeColor)
				break;
		}

		ImGui::Separator();
		ImGui::Checkbox("Color time", &startValues.timeColor);
		if (startValues.timeColor)
		{

			ImGui::DragFloat("Position", &nextPos, 1.0f, 1.0f, 100.0f, "%.2f");
			ImGui::ColorPicker4("", &nextColor.x, ImGuiColorEditFlags_AlphaBar);
			if (ImGui::Button("Add Color", ImVec2(125, 25)))
			{
				ColorTime colorTime;
				colorTime.color = nextColor;
				colorTime.position = nextPos / 100;
				colorTime.name = std::to_string((int)nextPos) + "%";
				startValues.color.push_back(colorTime);
				startValues.color.sort();
			}
			ImGui::Separator();
		}

		//Particle Texture
		if (texture)
		{
			std::string name = texture->file;
			name = name.substr(name.find_last_of("\\") + 1);

			ImGui::Text("Loaded texture '%s'", name.data());
			ImGui::Text("Texture used %i times", texture->usage);

			ImGui::Image((void*)(intptr_t)texture->GetID(), ImVec2(256, 256));

			if (ImGui::BeginMenu("Change Texture"))
			{
				std::vector<Resource*> resource;
				App->resources->GetResources(resource, ResourceType::texture);

				for (std::vector<Resource*>::iterator iterator = resource.begin(); iterator != resource.end(); ++iterator)
				{
					if (ImGui::MenuItem((*iterator)->name.data()))
					{
						App->resources->Remove(texture);
						texture = nullptr;

						texture = ((ResourceTexture*)(*iterator));
						texture->usage++;
					}
				}
				ImGui::End();
			}
			if (ImGui::Button("Remove", ImVec2(50, 25)))
			{
				App->resources->Remove(texture);
				texture = nullptr;
			}

		}
		else
		{
			ImGui::Text("No texture loaded");
			if (ImGui::BeginMenu("Add new Texture"))
			{
				std::vector<Resource*> resource;
				App->resources->GetResources(resource, ResourceType::texture);

				for (std::vector<Resource*>::iterator iterator = resource.begin(); iterator != resource.end(); ++iterator)
				{
					if (ImGui::MenuItem((*iterator)->name.data()))
					{
						texture = ((ResourceTexture*)(*iterator));
						texture->usage++;
					}
				}
				ImGui::End();
			}
			ImGui::Separator();
		}

		if (ImGui::Button("Remove Particles", ImVec2(150, 25)))
			toDelete = true;
	}
}

void ComponentEmitter::EditColor(ColorTime &colorTime)
{
	ImVec4 color = EqualsFloat4(colorTime.color);
	if (ImGui::ColorButton(colorTime.name.data(), color, ImGuiColorEditFlags_None, ImVec2(100, 20)))
		colorTime.changingColor = !colorTime.changingColor;

	if (!colorTime.changingColor)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(colorTime.name.data());
	}
	else
		ImGui::ColorEdit4(colorTime.name.data(), &colorTime.color.x, ImGuiColorEditFlags_AlphaBar);
}

ImVec4 ComponentEmitter::EqualsFloat4(const float4 float4D)
{
	ImVec4 vec;
	vec.x = float4D.x;
	vec.y = float4D.y;
	vec.z = float4D.z;
	vec.w = float4D.w;
	return vec;
}


// todo event system to delete texture