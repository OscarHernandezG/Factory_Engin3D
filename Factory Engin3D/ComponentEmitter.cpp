#include "ComponentEmitter.h"
#include "Application.h"
#include "ModuleTime.h"


ComponentEmitter::ComponentEmitter(GameObject* gameObject) : Component(gameObject, ComponentType_EMITTER)
{
}


ComponentEmitter::~ComponentEmitter()
{
}

void ComponentEmitter::Update()
{
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
}
