#ifndef __Camera_H__
#define __Camera_H__

#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

class GameObject;

class Camera : public Component
{
public:

	Camera(GameObject* gameObject);

	void Inspector();

	float4x4 GetViewMatrix();
	float4x4 GetProjectionMatrix();

	void UpdateFrustum();

	~Camera() {}


	void		SetPos(float x, float y, float z);
	void		SetPos(float3 pos);
	float3		GetPos();

	void SaveComponent(JSON_Object * parent);

public:
	Frustum frustum;
};

#endif // !__Camera_H__
