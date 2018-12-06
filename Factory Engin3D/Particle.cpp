#include "Application.h"
#include "Particle.h"
#include "Geometry.h"


Particle::Particle(float3 pos)
{
	plane = new ParticlePlane(pos);
}

Particle::~Particle()
{
	delete(plane);
}

float Particle::GetCamDistance() const
{
	return App->geometry->GetPlayingCamera()->GetPos().DistanceSq(transform.position);
}

void Particle::Draw() const
{
	plane->Render(transform.GetMatrix(),texture,color);
}


//Particle transform
float4x4 ParticleTrans::GetMatrix() const
{
	float4x4 transform = float4x4::FromTRS(position, rotation, scale);
	transform.Transposed();

	return transform;
}
