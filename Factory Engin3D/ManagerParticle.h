#ifndef __ManagerParticle_H__
#define __ManagerParticle_H__

#define MAX_PARTICLES 5000;

#include <list>

class ManagerParticle
{
public:
	ManagerParticle();
	~ManagerParticle();


public:
	std::list<Particle*> particleList;
};

#endif // !__ManagerParticle_H__
