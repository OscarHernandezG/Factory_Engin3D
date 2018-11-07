#ifndef __Camera_H__
#define __Camera_H__

#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class Camera : public Component
{
public:

	Camera(GameObject* gameObject);

	float4x4 GetViewMatrix();

	float4x4 GetProjectionMatrix();

	void UpdateFrustum();

	~Camera() {}


	void		SetPos(float x, float y, float z);
	void		SetPos(float3 pos);
	float3		GetPos();

public:
	Frustum frustum;


	AABB boundingBox;
};

#endif // !__Camera_H__
