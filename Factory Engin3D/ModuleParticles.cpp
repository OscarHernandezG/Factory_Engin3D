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

update_status ModuleParticle::Update()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	int count = 0;
	float dt = App->time->GetdtGame();
	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		if (allParticles[i].active)
		{
			allParticles[i].Update(dt);
			++count;
			allParticles[i].SetCamDistance();
			partQueue.push(&allParticles[i]);
		}
		else
		{
			allParticles[i].camDistance = -1;
		}
	}
		LOG("Active particles %i", count);
	return UPDATE_CONTINUE;
}

void ModuleParticle::Draw()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	if (plane == nullptr)
		plane = new ParticlePlane();

	DrawParticles();
}

void ModuleParticle::DrawParticles()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	while (!partQueue.empty())
	{
		Particle* currPart = partQueue.top();

		if (currPart->owner->gameObject->canDraw)
			currPart->Draw();

		partQueue.pop();
	}
}

/*void ModuleParticle::SortParticles()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	std::sort(&allParticles[0], &allParticles[MAX_PARTICLES]);
}
*/
bool ModuleParticle::GetParticle(int& id)
{
	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) 
	{
		if (!allParticles[i].active) 
		{
			lastUsedParticle = i;
			id = i;
			return true;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) 
	{
		if (!allParticles[i].active) 
		{
			lastUsedParticle = i;
			id = i;
			return true;
		}
	}

	return false;
}
