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

	~Transform() {}

	void Update(float dt);

	void UpdateBoundingBox();

//Position
//----------------------------------------------------------------
//	Set
	inline void SetPos(float x, float y, float z);
	inline void SetPos(float3 position);

	void Move(float3 position);
//	Get
	float3 GetPos() const;
	float3 GetLocalPos() const;
//----------------------------------------------------------------


// Scale
//----------------------------------------------------------------
//	Set
	inline void SetScale(float x, float y, float z);
	inline void SetScale(float3 scale);

	void Scale(float3 scale);
// Get
	float3 GetScale() const;
	float3 GetLocalScale() const;
//----------------------------------------------------------------
	
	
// Rotation
//----------------------------------------------------------------
//	Set
	inline void SetRotation(Quat rotation);
	inline void SetRotation(float3 rotation);

	void Rotate(Quat rotation);
// Get
	Quat GetRotation() const;
	Quat GetLocalRotation() const;
//----------------------------------------------------------------
	

// Transform
//----------------------------------------------------------------
	void SetTransform(float4x4 trans);
	inline void SetIdentity();
//----------------------------------------------------------------


// Matrix
//----------------------------------------------------------------
	float4x4	GetMatrixOGL() const;
	float4x4	GetMatrix() const;

	float4x4 GetLocalMatrix() const;
//----------------------------------------------------------------


	void SaveComponent(JSON_Object* parent);

public:
	AABB boundingBox;
	AABB originalBoundingBox;

private:
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;

};