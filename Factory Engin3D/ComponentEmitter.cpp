#include "Application.h"

#include "ComponentEmitter.h"



ComponentEmitter::ComponentEmitter(GameObject* gameObject) : Component(gameObject, ComponentType_EMITTER)
{
	timer.Start();
}


ComponentEmitter::~ComponentEmitter()
{
}

void ComponentEmitter::Update()
{
	if (timer.ReadSec() >  1 / rateOverTime && (App->particle->particleList.size() < 5000))//wtf
	{
		Particle* newParticle = new Particle(gameObject->transform->GetGlobalPos(), startValues, &texture);

		particles.push_back(newParticle);
		App->particle->particleList.push_back(newParticle);
	}

	for (std::list<Particle*>::iterator iterator = particles.begin(); iterator != particles.end(); ++iterator)
	{
		(*iterator)->Update();
	}
}

void ComponentEmitter::Inspector()
{



}


// todo event system to delete texture