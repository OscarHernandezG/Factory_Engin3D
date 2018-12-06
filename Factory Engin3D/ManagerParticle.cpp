#include "ManagerParticle.h"
#include <algorithm>


ManagerParticle::ManagerParticle()
{
}


ManagerParticle::~ManagerParticle()
{
}

void ManagerParticle::Draw()
{
	if (!particleList.empty())
	{
		particleList.sort(particleCompare());
		for (std::list<Particle*>::iterator iterator = particleList.begin(); iterator != particleList.end(); ++iterator)
		{
			(*iterator)->Draw();
		}
	}
}
