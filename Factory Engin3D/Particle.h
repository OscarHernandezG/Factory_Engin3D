#ifndef __Particle_H__
#define __Particle_H__

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

#include "ParticlePlane.h"
#include "ComponentTexture.h"

#include "ComponentEmitter.h"

struct ParticleTrans
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	float4x4 GetMatrix() const;
};

class Particle
{
public:
	Particle(float3 pos, StartValues data, Texture** texture);
	~Particle();

	void Update() {/*TODO*/};

	float GetCamDistance() const;
	void Draw() const;
public:
	float lifeTime = 0.0f;
	float life = 0.0f;
	float3 speed = float3::zero; 
	float rotationSpeed = 0.0f; // Todo

	float4 color = float4::one;

	ParticleTrans transform;

private:
	ParticlePlane* plane = nullptr;
	Texture** texture = nullptr;

};

struct particleCompare
{
	bool operator()(const Particle* particle1, const Particle* particle2) const
	{
		return particle1->GetCamDistance() < particle2->GetCamDistance();
	}
};
#endif // !__Particle_H__

