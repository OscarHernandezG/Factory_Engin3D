#include "Application.h"
#include "Particle.h"
#include "Geometry.h"
#include "ComponentEmitter.h"
#include "ModuleParticles.h"
#include "pcg-c-basic-0.9/pcg_basic.h"

Particle::Particle(float3 pos, StartValues data, ResourceTexture** texture)
{}

Particle::Particle()
{
}

Particle::~Particle()
{
	//delete plane;
}

void Particle::SetActive(float3 pos, StartValues data, ResourceTexture ** texture)
{
	plane = App->particle->plane;

	lifeTime = CreateRandomNum(data.life);

	life = 0.0f;

	speed = CreateRandomNum(data.speed);
	acceleration = CreateRandomNum(data.acceleration);
	direction = data.particleDirection;

	angle = CreateRandomNum(data.rotation) * DEGTORAD;
	angularVelocity = CreateRandomNum(data.angularVelocity) * DEGTORAD;
	angularAcceleration = CreateRandomNum(data.angularAcceleration) * DEGTORAD;

	transform.position = pos;
	transform.rotation = Quat::FromEulerXYZ(0, 0, 0); //Start rotation
	transform.scale = float3::one * CreateRandomNum(data.size);

	//LOG("life %f", lifeTime);
	//LOG("size %f", transform.scale.x);

	for (std::list<ColorTime>::iterator iter = data.color.begin(); iter != data.color.end(); ++iter)
		color.push_back(*iter);

	multicolor = data.timeColor;
	this->texture = texture;

	active = true;
	subEmiter = data.subEmiter;
	index = 0;
}

bool Particle::Update(float dt)
{
	bool ret = true;
	if (owner->simulatedGame == GameState_PAUSE)
		dt = 0;
	life += dt;
	if (life < lifeTime)
	{
		speed += acceleration * dt;
		transform.position += direction * (speed * dt);

		LookAtCamera();

		if (color.size() == 1 || !multicolor)
			currentColor = color.front().color;

		else if (index + 1 < color.size())
		{
			float lifeNormalized = life / lifeTime;
			if (color[index + 1].position > lifeNormalized)
			{
				float timeNormalized = (lifeNormalized - color[index].position) / (color[index + 1].position - color[index].position);
				if (color[index + 1].position == 0)
					timeNormalized = 0;
				//LOG("%i", index);
				currentColor = color[index].color.Lerp(color[index + 1].color, timeNormalized);
				//LERP Color
			}
			else
				index++;
		}
		else
			currentColor = color[index].color;

		angularVelocity += angularAcceleration * dt;
		angle += angularVelocity * dt;
		transform.rotation = transform.rotation.Mul(Quat::RotateZ(angle));
	}
	else
	{
		color.clear();
		if (subEmiter && owner->subEmiter && owner->subEmiter->HasComponent(ComponentType_EMITTER))
		{
			ComponentEmitter* emiter = (ComponentEmitter*)owner->subEmiter->GetComponent(ComponentType_EMITTER);
			emiter->CreateParticles(emiter->GetEmition(), emiter->normalShapeType, transform.position);
		}
		active = false;
		ret = false;
		owner->particles.remove(this);
	}


	return ret;
}

void Particle::LookAtCamera()
{
	float3 zAxis = -App->renderer3D->currentCam->frustum.front;
	float3 yAxis = App->renderer3D->currentCam->frustum.up;
	float3 xAxis = yAxis.Cross(zAxis).Normalized();

	transform.rotation.Set(float3x3(xAxis, yAxis, zAxis));
}

float Particle::GetCamDistance() const
{
	return App->renderer3D->currentCam->GetPos().DistanceSq(transform.position);
}

void Particle::SetCamDistance()
{
	 camDistance = App->renderer3D->currentCam->GetPos().DistanceSq(transform.position);
}

void Particle::Draw() const
{
	if (plane && texture)
		plane->Render(transform.GetMatrix(), *texture, currentColor);
	
}


float Particle::CreateRandomNum(float2 edges)//.x = minPoint & .y = maxPoint
{
	float num = edges.x;
	if (edges.x < edges.y)
	{
		float random = (float)pcg32_random();	
		num = ((edges.y - edges.x) * random / (float)MAXUINT) + edges.x;
	}
	return num;
}

//Particle transform
float4x4 ParticleTrans::GetMatrix() const
{
	return  float4x4::FromTRS(position, rotation, scale).Transposed();
}
