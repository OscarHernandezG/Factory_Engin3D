#pragma once

#include "Component.h"

#include "MathGeoLib/MathGeoLib.h"

class GameObject;

enum UsingInfo
{
	UsingInfo_TRS,
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


	inline void			SetPos(float x, float y, float z);
	inline void			SetPos(float3 position);

	inline void			SetRotation(Quat rotation);
	inline void			Rotate(Quat rotation);

	inline void			SetScale(float x, float y, float z);
	inline void			SetIdentity();

	 float3			GetPos() const;
	inline float3	GetLocalPos() const;

	 Quat			GetRotation() const;
	inline Quat		GetLocalRotation() const;


	float4x4		GetMatrix() const;

public:

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	AABB boundingBox;
};