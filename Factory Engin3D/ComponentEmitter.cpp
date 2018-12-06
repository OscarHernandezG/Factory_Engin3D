#include "ComponentEmitter.h"



ComponentEmitter::ComponentEmitter()
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
