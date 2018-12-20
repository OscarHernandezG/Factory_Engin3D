#include "Application.h"

#include "ComponentEmitter.h"
#include "Application.h"
#include "ModuleTime.h"

#include <vector>

#include "ModuleParticles.h"

ComponentEmitter::ComponentEmitter(GameObject* gameObject) : Component(gameObject, ComponentType_EMITTER)
{
	timer.Start();
	burstTime.Start();
	App->sceneIntro->octree.Insert(gameObject);
}


ComponentEmitter::~ComponentEmitter()
{
}

void ComponentEmitter::StartEmitter()
{
	timer.Start();
	burstTime.Start();
	loopTimer.Start();

	timeToParticle = 0.0f;
}


void ComponentEmitter::Update()
{
	if (rateOverTime > 0)
	{
		float time = timer.ReadSec();
		if (time > timeToParticle && (loop || loopTimer.ReadSec() < duration))
		{
			if (App->time->gameState == GameState_PLAYING || simulatedGame == GameState_PLAYING)
			{
				int particlesToCreate = (time / (1.0f / rateOverTime));
				CreateParticles(particlesToCreate);

				timeToParticle = (1.0f / rateOverTime);

				timer.Start();
			}

		}
	}
	float burstT = burstTime.ReadSec();
	if (burst && burstT > repeatTime)
	{
		if (App->time->gameState == GameState_PLAYING || simulatedGame == GameState_PLAYING)
		{
			int particlesToCreate = minPart;
			if (minPart != maxPart)
				particlesToCreate = (rand() % (maxPart - minPart)) + minPart;
			CreateParticles(particlesToCreate);
			//LOG("%i", particlesToCreate);
		}
		burstTime.Start();
	}

	// Use this condition to remove all particles from the component Emitter
	if (!emitterActive)
	{
		for (std::list<Particle*>::iterator iterator = particles.begin(); iterator != particles.end(); ++iterator)
		{
			(*iterator)->active = false;
		}

		particles.clear();
	}


	//std::vector<Particle*> particleDelete;
	//for (std::list<Particle*>::iterator iterator = particles.begin(); iterator != particles.end(); ++iterator)
	//{
	//	if (!(*iterator)->Update(App->time->GetdtGame()) || toDelete)
	//		particleDelete.push_back(*iterator);
	//}

	//for (std::vector<Particle*>::iterator iterator = particleDelete.begin(); iterator != particleDelete.end(); ++iterator)
	//{
	//	particles.remove(*iterator);
	//	App->particle->particleList.remove(*iterator);

	//	delete *iterator;
	//}

}

void ComponentEmitter::Revive(float3 pos)
{
	startValues.revive = false;
	int particlesToCreate = minPart;
	if (minPart != maxPart)
		particlesToCreate = (rand() % (maxPart - minPart)) + minPart;
	CreateParticles(particlesToCreate, pos);

}

void ComponentEmitter::CreateParticles(int particlesToCreate, float3 pos)
{
	if (particlesToCreate == 0)
		++particlesToCreate;

	for (int i = 0; i < particlesToCreate; ++i)
	{
		int particleId = 0;
		if (App->particle->GetParticle(particleId))
		{
			if(pos.x == float3::zero.x && pos.y == float3::zero.y && pos.z == float3::zero.z)
				pos = RandPos();

			App->particle->allParticles[particleId].SetActive(pos, startValues, &texture);

			App->particle->allParticles[particleId].owner = this;
			particles.push_back(&App->particle->allParticles[particleId]);
		}
		else
			break;
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
		if (ImGui::DragFloat2("Speed", &startValues.speed.x, 0.25f, 0.25f, 20.0f, "%.2f"))
			CheckMinMax(startValues.speed);
		if(ImGui::DragFloat2("Acceleration", &startValues.acceleration.x, 0.25f, -5.0f, 5.0f, "%.2f"))
			CheckMinMax(startValues.acceleration);

		if(ImGui::DragFloat2("Rotation", &startValues.rotation.x, 0.25f, -720.0f, 720.0f, "%.2f"))
			CheckMinMax(startValues.rotation);
		if(ImGui::DragFloat2("Angular acceleration", &startValues.angularAcceleration.x, 0.25f, -45.0f, 45.0f, "%.2f"))
			CheckMinMax(startValues.angularAcceleration);

		if(ImGui::DragFloat2("Lifetime", &startValues.life.x, 0.5f, 1.0f, 20.0f, "%.2f"))
			CheckMinMax(startValues.life);
		if(ImGui::DragFloat2("Size", &startValues.size.x, 0.1f, 0.1f, 5.0f, "%.2f"))
			CheckMinMax(startValues.size);
		ImGui::DragInt("Emition", &rateOverTime, 1.0f, 0.0f, 300.0f, "%.2f");

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
		ImGui::SameLine(); 
		ImGui::ShowHelpMarker("Click color square for change it");
		std::vector<ColorTime> deleteColor;
		std::list<ColorTime>::iterator iter = startValues.color.begin();
		while (iter != startValues.color.end())
		{
			//TODO: they must be able to change position
			if ((iter) == startValues.color.begin())
			{//Cant delete 1st color
				if (!EditColor(*iter))
					break;
				iter++;
			}
			else
			{
				if (!EditColor(*iter, false))
					startValues.color.erase(iter++);
				else
					iter++;
			}
		}
		ImGui::Separator();
		ImGui::Checkbox("Color time", &startValues.timeColor);
		if (startValues.timeColor)
		{

			ImGui::DragInt("Position", &nextPos, 1.0f, 1, 100);
			ImGui::ColorPicker4("", &nextColor.x, ImGuiColorEditFlags_AlphaBar);
			if (ImGui::Button("Add Color", ImVec2(125, 25)))
			{
				ColorTime colorTime;
				colorTime.color = nextColor;
				colorTime.position = (float)nextPos / 100;
				colorTime.name = std::to_string((int)nextPos) + "%";
				startValues.color.push_back(colorTime);
				startValues.color.sort();
			}
		}
		ImGui::Separator();

		ImGui::Checkbox("Revive", &startValues.revive);
		ImGui::Checkbox("Burst", &burst);
		if (burst || startValues.revive)
		{
			ImGui::DragInt("Min particles", &minPart, 1.0f, 0, 100);
			if (minPart > maxPart)
				maxPart = minPart;
			ImGui::DragInt("Max Particles", &maxPart, 1.0f, 0, 100);
			if (maxPart < minPart)
				minPart = maxPart;
			ImGui::DragFloat("Repeat Time", &repeatTime, 0.5f, 0.0f, 0.0f, "%.1f");
		}
		ImGui::Separator();


		ImGui::Separator();
		ImGui::Checkbox("Bounding Box", &drawAABB);
		if (drawAABB)
		{
			float3 size = gameObject->transform->originalBoundingBox.Size();
			if (ImGui::DragFloat3("Dimensions", &size.x, 1.0f, 0.0f, 0.0f, "%.0f"))
			{
				gameObject->transform->originalBoundingBox.SetFromCenterAndSize(posDifAABB, size);
				gameObject->transform->UpdateBoundingBox();
			}

			if(ImGui::DragFloat3("Pos", &posDifAABB.x, 1.0f, 0.0f, 0.0f, "%.0f"))
			{
				gameObject->transform->originalBoundingBox.SetFromCenterAndSize(posDifAABB, size);
				gameObject->transform->UpdateBoundingBox();
			}
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

void ComponentEmitter::CheckMinMax(float2& value)
{
	if (value.x > value.y)
		value.y = value.x;
}

bool ComponentEmitter::EditColor(ColorTime &colorTime, bool first)
{
	bool ret = true;
	ImVec4 color = EqualsFloat4(colorTime.color);
	if (ImGui::ColorButton(colorTime.name.data(), color, ImGuiColorEditFlags_None, ImVec2(100, 20)))
		colorTime.changingColor = !colorTime.changingColor;

	if (!colorTime.changingColor)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(colorTime.name.data());
		if (!first)
		{
			ImGui::SameLine();
			if (ImGui::Button("Remove Color", ImVec2(100, 25)))
				ret = false;
		}
		else if (!startValues.timeColor)
			ret = false;
	}
	else
		ImGui::ColorEdit4(colorTime.name.data(), &colorTime.color.x, ImGuiColorEditFlags_AlphaBar);
	return ret;
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