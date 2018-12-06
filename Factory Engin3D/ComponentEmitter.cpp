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
	if (timer.ReadSec() >  1 / rateOverTime && (App->particle->particleList.size() < MAX_PARTICLES))
	{
		Particle* newParticle = new Particle(gameObject->transform->GetGlobalPos(), startValues, &texture);

		particles.push_back(newParticle);
		App->particle->particleList.push_back(newParticle);
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

void ComponentEmitter::Inspector()
{
	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}


// todo event system to delete texture