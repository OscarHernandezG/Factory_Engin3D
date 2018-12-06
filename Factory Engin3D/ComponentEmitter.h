#ifndef __Emitter_H__
#define __Emitter_H__

#include "Component.h"
#include "GameObject.h"

#include "Timer.h"

#include <list>

struct StartValues
{
	// Start values
	float life = 5.0f;
	float speed = 5.0f;
	float size = 1.0f;
	float rotation = 0.0f;
	float4 color = float4::one;
};

class ComponentEmitter : Component
{
public:
	ComponentEmitter(GameObject* gameObject);
	~ComponentEmitter();

	void Update();
	void Inspector();

public:
	Timer timer;

	std::list<Particle*> particles;

	// Particle texture
	Texture* texture;

	// General info
	//---------------------------------------
	// Duration of the particle emitter
	float duration = 5.0f;
	
	// Loop the particle (if true the particle emitter will never stop)
	bool loop = true;
	// Warm up the particle emiter (if true the particle emitter will be already started at play-time)
	bool preWarm = true;

	float gravity = 0.0f;

	StartValues startValues;
	//---------------------------------------

	// Emission info
	//---------------------------------------
	// Number of particles created per second
	uint rateOverTime = 10;
	//---------------------------------------

	
};
#endif // !__Emitter_H__