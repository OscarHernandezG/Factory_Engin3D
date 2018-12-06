#ifndef __Particle_H__
#define __Particle_H__

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

#include "ParticlePlane.h"
#include "Texture.h"

struct ParticleTrans
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;
};

class Particle
{
public:
	Particle();
	~Particle();

public:
	float lifeTime = 0.0f;
	float life = 0.0f;
	float3 speed = float3::zero;
	float rotationSpeed = 0.0f;

	float3 color = float3::one;

	ParticleTrans transform;

private:
	ParticlePlane* plane = nullptr;
	Texture* texture = nullptr;
};
#endif // !__Particle_H__

