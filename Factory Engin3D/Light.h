
#pragma once
#include "Color.h"
#include "MathGeoLib/Math/float3.h"


struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	float3 position;

	int ref;
	bool on;
};




// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
// TEMP CAMERA COMPONENT


#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class Camera : public Component
{
public:

	Camera(GameObject* gameObject);

	float4x4 GetViewMatrix();

	float* GetProjectionMatrix();

	~Camera() {}


	void		SetPos(float x, float y, float z);
	void		SetRotation(float angle, const float3 &u);
	void		SetScale(float x, float y, float z);
	void		SetIdentity();


public:
	Frustum frustum;
	
		
	AABB boundingBox;
};