#ifndef __Emitter_H__
#define __Emitter_H__

#include "Component.h"
#include "GameObject.h"

#include "Timer.h"
#include "GameTimer.h"

#include "Particle.h"

#include <list>

enum ShapeType {
	ShapeType_BOX,
	ShapeType_SPHERE,
	ShapeType_SPHERE_CENTER,
	ShapeType_SPHERE_BORDER
};

struct ColorTime
{
	float4 color = float4::one;
	float position = 0.0f;
	std::string name = " ";
	//open window for change particle color
	bool changingColor = false;

	bool operator<(const ColorTime &color) const
	{
		return position < color.position;
	}
};

struct StartValues
{
	// Start values
	float2 life = float2(0.0f, 5.0f);
	float2 speed = float2(1.5f, 3.0f);
	float2 acceleration = float2(0.0f, 0.0f);
	float2 size = float2(1.0f, 2.0f);
	float2 rotation = float2(0.0f, 1.0f);
	float2 angularAcceleration = float2(0.0f, 0.0f);
	std::list<ColorTime> color;
	bool timeColor = false;
	bool revive = false;

	AABB colision = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));

	float3 particleDirection = float3::unitY;

	StartValues()
	{
		ColorTime colorTime;
		colorTime.name = "Start Color";
		color.push_back(colorTime);
	}
};

struct EmitterInfo : ComponentInfo
{

	float duration = 1.0f;

	bool loop = true;


	bool burst = false;
	int minPart = 0;
	int maxPart = 10;
	float repeatTime = 1.0f;

	float3 posDifAABB = float3::zero;
	float gravity = 0.0f;

	AABB boxCreation = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));
	Sphere SphereCreation = Sphere(float3::zero, 1.0f);

	ShapeType shapeType = ShapeType_BOX;

	ResourceTexture* texture = nullptr;

	StartValues startValues;
};

class ComponentEmitter : public Component
{
public:
	ComponentEmitter(GameObject* gameObject);
	ComponentEmitter(GameObject* gameObject, EmitterInfo* info);
	~ComponentEmitter();

	void StartEmitter();

	void Update();
	float3 RandPos();
	void Inspector();

	void ShowFloatValue(float2 & value, bool checkBox, const char * name, float v_speed, float v_min, float v_max);
	void CheckMinMax(float2 & value);
	void Revive(float3 pos);
	void ClearEmitter();
	void SoftClearEmitter();
	void CreateParticles(int particlesToCreate, float3 pos = float3::zero);
	bool EditColor(ColorTime & colorTime, bool first = true);

	ImVec4 EqualsFloat4(const float4 float4D);

	void SaveComponent(JSON_Object * parent);

public:
	Timer timer;
	Timer burstTime;

	// Particle texture
	ResourceTexture* texture = nullptr;

	bool drawAABB = false;

	// Emitter particles
	std::list<Particle*> particles;

	bool emitterActive = true;

	GameState simulatedGame = GameState_NONE;
	GameTimer timeSimulating;

private:
	// General info
	//---------------------------------------
	// Duration of the particle emitter
	float duration = 1.0f;

	//Check box Randomize values
	bool checkLife = false;
	bool checkSpeed = false;
	bool checkAcceleration = false;
	bool checkSize = false;
	bool checkRotation = false;
	bool checkAngularAcceleration = false;

	// Loop the particle (if true the particle emitter will never stop)
	bool loop = true;
	Timer loopTimer;
	// Warm up the particle emiter (if true the particle emitter will be already started at play-time)
	bool preWarm = true;

	//Burst options
	bool burst = false;
	int minPart = 0;
	int maxPart = 10;
	float repeatTime = 1.0f;

	float3 posDifAABB = float3::zero;
	float gravity = 0.0f;

	//Posibility space where particle is created
	AABB boxCreation = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));
	Sphere SphereCreation = Sphere(float3::zero, 1.0f);

	ShapeType shapeType = ShapeType_BOX;

	int nextPos = 100;
	float4 nextColor = float4::zero;

	StartValues startValues;
	//---------------------------------------

	// Emission info
	//---------------------------------------
	// Number of particles created per second
	int rateOverTime = 10;
	float timeToParticle = 0.0f;
	//---------------------------------------

};
#endif // !__Emitter_H__