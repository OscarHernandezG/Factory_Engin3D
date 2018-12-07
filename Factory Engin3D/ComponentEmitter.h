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

	float3 particleDirection = float3::unitY;
};

enum ShapeType {
	ShapeType_BOX,
	ShapeType_SPHERE
};
class ComponentEmitter : Component
{
public:
	ComponentEmitter(GameObject* gameObject);
	~ComponentEmitter();

	void Update();
	float3 RandPos();
	void Inspector();


	ImVec4 EqualsFloat4(const float4 float4D);

public:
	Timer timer;

	std::list<Particle*> particles;

	// Particle texture
	Texture* texture = nullptr;

private:
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
	ShapeType shapeType = ShapeType_BOX;

	//open window for change particle color
	bool changingColor = false;

	StartValues startValues;
	//---------------------------------------

	// Emission info
	//---------------------------------------
	// Number of particles created per second
	int rateOverTime = 10;
	//---------------------------------------

	
};
#endif // !__Emitter_H__