#include "Application.h"
#include "Particle.h"
#include "Geometry.h"


Particle::Particle()
{
}

Particle::~Particle()
{
}

float Particle::GetCamDistance() const
{
	return App->geometry->GetPlayingCamera()->GetPos().DistanceSq(transform.position);
}

void Particle::Draw() const
{
	plane->Render();
}
