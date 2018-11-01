#ifndef CAMERA_H
#define CAMERA_H

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

#endif
