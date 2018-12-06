#include "ComponentEmitter.h"



ComponentEmitter::ComponentEmitter(GameObject* gameObject) : Component(gameObject, ComponentType_EMITTER)
{
}


ComponentEmitter::~ComponentEmitter()
{
}

void ComponentEmitter::Update()
{
	for (std::list<Particle*>::iterator iterator = particles.begin(); iterator != particles.end(); ++iterator)
	{
		(*iterator)->Update();
	}
}

void ComponentEmitter::Inspector()
{
}
