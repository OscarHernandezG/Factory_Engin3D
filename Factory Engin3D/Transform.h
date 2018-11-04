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

	void Update(float dt);

	void UptadeBoundingBox();

	void		SetPos(float x, float y, float z);
	void		SetPos(float3 position);

	void		SetRotation(Quat rotation);
	void		SetRotation(float3 rotation);
	void		Rotate(Quat rotation);

	void		SetScale(float x, float y, float z);
	void		SetIdentity();

	float3		GetPos() const;
	float3		GetLocalPos() const;

	float3		GetScale() const;

	Quat		GetRotation() const;
	Quat		GetLocalRotation() const;


	float4x4	GetMatrix() const;

public:

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	AABB boundingBox;
	AABB originalBoundingBox;
};