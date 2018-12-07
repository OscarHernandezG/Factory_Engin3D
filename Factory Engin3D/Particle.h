#ifndef __Particle_H__
#define __Particle_H__

#include "MathGeoLib/Math/MathAll.h"

#include "ParticlePlane.h"

struct StartValues;
struct ColorTime;
struct ParticleTrans
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	//AABB colision = AABB(-float3::one,float3::one);
	float4x4 GetMatrix() const;
};

class Particle
{
public:
	Particle(float3 pos, StartValues data, ResourceTexture** texture);
	~Particle();

	bool Update(float dt);

	void LookAtCamera();

	float GetCamDistance() const;
	void Draw() const;
public:
	float lifeTime = 0.0f;
	float life = 0.0f;
	float3 speed = float3::zero; 
	float rotationSpeed = 0.0f; // Todo

	std::list<ColorTime> color;
	bool oneColor = false;
	float4 currentColor = float4::one;

	ParticleTrans transform;

private:
	ParticlePlane* plane = nullptr;
	ResourceTexture** texture = nullptr;

};

struct particleCompare
{
	bool operator()(const Particle* particle1, const Particle* particle2) const
	{
		return particle1->GetCamDistance() > particle2->GetCamDistance();
	}
};
#endif // !__Particle_H__

