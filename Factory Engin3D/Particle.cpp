#include "Application.h"
#include "Particle.h"
#include "Geometry.h"
#include "ComponentEmitter.h"

Particle::Particle(float3 pos, StartValues data, ResourceTexture** texture)
{
	plane = new ParticlePlane();

	lifeTime = data.life;
	speed = data.particleDirection * data.speed;

	transform.position = pos;
	transform.rotation = Quat::FromEulerXYZ(0, 0, 0); //data.rotation;
	transform.scale = float3::one * data.size;
	//transform.colision = data.colision;

	color = data.color;

	this->texture = texture;	
}

Particle::~Particle()
{
	delete(plane);
}

bool Particle::Update(float dt)
{
	bool ret = true;
	transform.position += speed * dt;

	life += dt;
	if (life >= lifeTime)
	{
		ret = false;
	}
	return ret;
}

float Particle::GetCamDistance() const
{
	return App->renderer3D->currentCam->GetPos().Distance(transform.position);
}

void Particle::Draw() const
{
	if (texture)
		plane->Render(transform.GetMatrix(), *texture, color);
	
}


//Particle transform
float4x4 ParticleTrans::GetMatrix() const
{
	return  float4x4::FromTRS(position, rotation, scale).Transposed();
}
