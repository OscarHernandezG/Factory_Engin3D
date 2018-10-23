#pragma once
#include "Component.h"
#include "GameObject.h"

enum UsingInfo
{
	UsingInfo_TRS,
	UsingInfo_TRANSFORM
};

struct TransformInfo : ComponentInfo
{
	float3 position = float3::zero;
	Quat rotation = Quat::identity;

	float3 scale = float3::one;

	float4x4 transform = float4x4::identity;

	UsingInfo whichInfo = UsingInfo_TRS;

	GameObject* gameObject = nullptr;
};

class Transform : Component
{
public:
	Transform(TransformInfo* info);
	Transform(GameObject* gameObject) : Component(gameObject, ComponentType_TRANSFORM), transform(float4x4::identity) {}
	Transform(GameObject* gameObject, float4x4 transform) : Component(gameObject, ComponentType_TRANSFORM), transform(transform) {}

	~Transform() {}

public:

	float4x4 transform;
};
