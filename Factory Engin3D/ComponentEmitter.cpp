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

	if (timer.ReadSec() >  1 / rateOverTime && (App->particle->particleList.size() < MAX_PARTICLES) && (loop || loopTimer.ReadSec() < duration) )
	{
		Particle* newParticle = new Particle(RandPos(), startValues, &texture);

		particles.push_back(newParticle);
		App->particle->particleList.push_back(newParticle);
		timer.Start();
	}

	std::vector<Particle*> toDelete;
	for (std::list<Particle*>::iterator iterator = particles.begin(); iterator != particles.end(); ++iterator)
	{
		if (!(*iterator)->Update(App->time->GetdtGame()))
			toDelete.push_back(*iterator);
	}

	for (std::vector<Particle*>::iterator iterator = toDelete.begin(); iterator != toDelete.end(); ++iterator)
	{
		particles.remove(*iterator);
		App->particle->particleList.remove(*iterator);

		delete *iterator;
	}

}

float3 ComponentEmitter::RandPos()
{

	//TODO: create random pos from AABB creation
	return gameObject->transform->GetGlobalPos();
}

void ComponentEmitter::Inspector()
{
	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat("Speed", &startValues.speed, 0.25f, 0.25f, 20.0f);
		ImGui::DragInt("Particles per Second", &rateOverTime, 1.0f, 1, 50);
		if (ImGui::Checkbox("Loop", &loop))
			loopTimer.Start();
	}
}


// todo event system to delete texture