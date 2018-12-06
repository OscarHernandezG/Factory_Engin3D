#ifndef __Emitter_H__
#define __Emitter_H__

#include "Component.h"
#include "GameObject.h"

#include "Particle.h"



#include <list>

class ComponentEmitter : Component
{
public:
	ComponentEmitter(GameObject* gameObject);
	~ComponentEmitter();

	void Update();
	void Inspector();

public:

	std::list<Particle*> particles;

	// General info
	//---------------------------------------
	// Duration of the particle emitter
	float duration = 5.0f;
	
	// Loop the particle (if true the particle emitter will never stop)
	bool loop = true;
	// Warm up the particle emiter (if true the particle emitter will be already started at play-time)
	bool preWarm = true;

	float gravity = 0.0f;

	// Start values
	float startLife = 5.0f;
	float startSpeed = 5.0f;
	float startSize = 1.0f;
	float startRotation = 0.0f;
	float4 startColor = float4::one;
	//---------------------------------------

	// Emission info
	//---------------------------------------
	// Number of particles created per second
	uint rateOverTime = 10;
	//---------------------------------------

	
};
#endif // !__Emitter_H__