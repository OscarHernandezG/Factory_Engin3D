#ifndef __MouduleParticle_H__
#define __MouduleParticle_H__

#define MAX_PARTICLES 500

#include "Application.h"

#include <list>
#include "Particle.h"

class ModuleParticle : public Module
{
public:
	ModuleParticle(Application* app, bool start_enabled = true);
	~ModuleParticle();

	void Draw();
public:
	std::list<Particle*> particleList;

	ParticlePlane* plane = nullptr;
};

#endif // !__MouduleParticle_H__
