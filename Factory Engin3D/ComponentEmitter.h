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
	float speed = 3.0f;
	float size = 1.0f;
	//float rotation = 0.0f;
	float4 color = float4::one;
	AABB colision = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));
};

class ComponentEmitter : Component
{
public:
	ComponentEmitter(GameObject* gameObject);
	~ComponentEmitter();

	void Update();
	float3 RandPos();
	void Inspector();

public:
	Timer timer;

	std::list<Particle*> particles;

	// Particle texture
	Texture* texture = nullptr;

	// General info
	//---------------------------------------
	// Duration of the particle emitter
	float duration = 1.0f;
	
	// Loop the particle (if true the particle emitter will never stop)
	bool loop = true;
	Timer loopTimer;
	// Warm up the particle emiter (if true the particle emitter will be already started at play-time)
	bool preWarm = true;

	float gravity = 0.0f;

	//Posibility space where particle is created
	AABB creation = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));

	//open window for change particle color
	bool changingColor = false;
	//float4 color in ImGui
	ImVec4 color = ImVec4(1.0f,1.0f,1.0f,1.0f);

	StartValues startValues;
	//---------------------------------------

	// Emission info
	//---------------------------------------
	// Number of particles created per second
	int rateOverTime = 10;
	//---------------------------------------

	
};
#endif // !__Emitter_H__