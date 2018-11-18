#ifndef __Camera_H__
#define __Camera_H__

#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

class GameObject;

struct CameraInfo : ComponentInfo
{
	float nearPlaneDistance = 0.0f;
	float farPlaneDistance = 0.0f;
	float verticalFov = 0.0f;
	float horizontalFov = 0.0f;

	float3 position = float3::zero;
};

class Camera : public Component
{
public:
	Camera(GameObject* gameObject);
	Camera(GameObject* gameObject, CameraInfo * info);

	void Inspector();

	float4x4 GetViewMatrix();
	float4x4 GetProjectionMatrix();

	~Camera() {}


	void		SetPos(float x, float y, float z);
	void		SetPos(float3 pos);
	float3		GetPos();

	void SaveComponent(JSON_Object * parent);

public:
	Frustum frustum;
};

#endif // !__Camera_H__
