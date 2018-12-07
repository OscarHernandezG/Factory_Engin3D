#include "Application.h"
#include "Particle.h"
#include "Geometry.h"
#include "ComponentEmitter.h"

Particle::Particle(float3 pos, StartValues data, ResourceTexture** texture)
{
	plane = new ParticlePlane();

	lifeTime = data.life;
	speed = data.particleDirection * data.speed;

	rotation = data.rotation;

	rotation *= DEGTORAD;

	transform.position = pos;
	transform.rotation = Quat::FromEulerXYZ(0, 0, 0); //Start rotation
	transform.scale = float3::one * data.size;

	color = data.color;

	this->texture = texture;	
}

Particle::~Particle()
{
	delete plane;
}

bool Particle::Update(float dt)
{
	bool ret = true;
	life += dt;
	if (life >= lifeTime)
	{
		ret = false;
	}
	else
	{
		transform.position += speed * dt;

		float3 zAxis = -App->renderer3D->currentCam->frustum.front;
		float3 yAxis = App->renderer3D->currentCam->frustum.up;
		float3 xAxis = yAxis.Cross(zAxis).Normalized();
		
		transform.rotation.Set(float3x3(xAxis, yAxis, zAxis));

		angle += rotation * dt;
		transform.rotation = transform.rotation.Mul(Quat::RotateZ(angle));
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
