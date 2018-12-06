#ifndef __Particle_H__
#define __Particle_H__

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

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
	float lifeTime;
	float life;
	float3 speed;
	float rotationSpeed;



};
#endif // !__Particle_H__

