#include "Application.h"
#include "Particle.h"
#include "Geometry.h"
#include "ComponentEmitter.h"

Particle::Particle(float3 pos, StartValues data, ResourceTexture** texture)
{
	plane = App->particle->plane;

	lifeTime = data.life;

	speed = data.speed;
	acceleration = data.acceleration;
	direction = data.particleDirection;

	rotation = data.rotation * DEGTORAD;
	angularAcceleration = data.angularAcceleration * DEGTORAD;

	transform.position = pos;
	transform.rotation = Quat::FromEulerXYZ(0, 0, 0); //Start rotation
	transform.scale = float3::one * data.size;

	for (std::list<ColorTime>::iterator iter = data.color.begin(); iter != data.color.end(); ++iter)
		color.push_back(*iter);

	multicolor = data.timeColor;
	this->texture = texture;	
}

Particle::~Particle()
{
	//delete plane;
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
				LOG("%i", index);
				currentColor = color[index].color.Lerp(color[index + 1].color,timeNormalized);
				//LERP Color
			}
			else
				index++;
		}
		else
			currentColor = color[index].color;

		rotation += angularAcceleration *dt;
		angle += rotation * dt;
		transform.rotation = transform.rotation.Mul(Quat::RotateZ(angle));
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
	return App->renderer3D->currentCam->GetPos().Distance(transform.position);
}

void Particle::Draw() const
{
	if (plane && texture)
		plane->Render(transform.GetMatrix(), *texture, currentColor);
	
}


//Particle transform
float4x4 ParticleTrans::GetMatrix() const
{
	return  float4x4::FromTRS(position, rotation, scale).Transposed();
}
