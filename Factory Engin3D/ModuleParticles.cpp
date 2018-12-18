#include "ModuleParticles.h"
#include <algorithm>

#include "Brofiler/Brofiler.h"

ModuleParticle::ModuleParticle(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleParticle::~ModuleParticle()
{
	delete plane;
}

void ModuleParticle::Draw()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	if (plane == nullptr)
		plane = new ParticlePlane();
	
	if (!particleList.empty())
	{
		particleList.sort(particleCompare());
		for (std::list<Particle*>::iterator iterator = particleList.begin(); iterator != particleList.end(); ++iterator)
		{
			(*iterator)->Draw();
		}
	}
}
