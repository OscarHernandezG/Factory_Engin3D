#ifndef __Transform_H__
#define __Transform_H__

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

	~Transform() {}

	void		Update(float dt);

	void		UpdateBoundingBox();

	void		SetPos(float x, float y, float z);
	void		SetPos(float3 position);

	void		Move(float3 position);
	void		SetTransform(float4x4 trans);
	void		Scale(float3 scale);

	void		SetRotation(Quat rotation);
	void		SetRotation(float3 rotation);
	void		Rotate(Quat rotation);

	void		SetScale(float x, float y, float z);
	void		SetScale(float3 scale);
	void		SetIdentity();

	 float3	GetPos() const;
	 float3	GetLocalPos() const;

	 float3	GetScale() const;

	 Quat		GetRotation() const;
	 Quat		GetLocalRotation() const;


	const float4x4	GetMatrixOGL() const;
	const float4x4	GetMatrix() const;

	const float4x4 GetGlobalMatrix() const;

	void SaveComponent(JSON_Object* parent);

public:

	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

	AABB boundingBox;
	AABB originalBoundingBox;
};
#endif