#pragma once

#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

class GameObject;

enum UsingInfo
{
	UsingInfo_TRS,
	UsingInfo_TRANSFORM
};

//Use xyz rotate euler angles

struct TransformInfo : ComponentInfo
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;

	float3 scale = float3::one;

	float4x4 transform = float4x4::identity;

	UsingInfo whichInfo = UsingInfo_TRS;

	GameObject* gameObject = nullptr;
};

class Transform : public Component
{
public:

	Transform(TransformInfo* info);
	Transform(GameObject* gameObject); 
	Transform(GameObject* gameObject, float4x4 transform);

	~Transform() {}


	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const float3 &u);
	void			SetScale(float x, float y, float z);
	void			SetIdentity();

	float3			GetPos() const;

public:
	float4x4 matrix;

	AABB boundingBox;
};