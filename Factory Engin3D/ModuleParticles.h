#ifndef __MouduleParticle_H__
#define __MouduleParticle_H__

#define MAX_PARTICLES 1000

#include "Application.h"

#include <list>
#include "Particle.h"

#include "ComponentEmitter.h"


class ModuleParticle : public Module
{
public:
	ModuleParticle(Application* app, bool start_enabled = true);
	~ModuleParticle();

	update_status Update();

	void Draw();
	void DrawParticles();
	void SortParticles();
	bool GetParticle(int& id);
public:

	ComponentEmitter* emitter = nullptr;

	Particle allParticles[MAX_PARTICLES];



	ParticlePlane* plane = nullptr;

private:

	int lastUsedParticle = 0;
};

#endif // !__MouduleParticle_H__
