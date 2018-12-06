#include "Application.h"
#include "Particle.h"
#include "Geometry.h"
#include "ComponentEmitter.h"

Particle::Particle(float3 pos, StartValues data, Texture** texture)
{
	plane = new ParticlePlane(pos);

	lifeTime = data.life;
	speed = float3::unitX * data.speed;

	transform.position = pos;
	transform.rotation = Quat::FromEulerXYZ(0, 0, 0); //data.rotation;
	transform.scale = float3::one * data.size;

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
	return App->geometry->GetPlayingCamera()->GetPos().DistanceSq(transform.position);
}

void Particle::Draw() const
{
	if (texture)
	plane->Render(transform.GetMatrix(),*texture,color);
}


//Particle transform
float4x4 ParticleTrans::GetMatrix() const
{
	float4x4 transform = float4x4::FromTRS(position, rotation, scale);
	transform.Transposed();

	return transform;
}
