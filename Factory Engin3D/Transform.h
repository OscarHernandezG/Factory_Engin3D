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

	float3 minBB = float3::zero, maxBB = float3::zero;
	float3 minOBB = float3::zero, maxOBB = float3::zero;
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
	
	void SetPos(float x, float y, float z);
	void SetPos(float3 position);

	void Move(float3 position);
//	Get
	float3 GetPos() const;
	float3 GetGlobalPos() const;
//----------------------------------------------------------------


// Scale
//----------------------------------------------------------------
//	Set
	void SetScale(float x, float y, float z);
	void SetScale(float3 scale);

	void Scale(float3 scale);
// Get
	float3 GetScale() const;
	float3 GetGlobalScale() const;
//----------------------------------------------------------------
	
	
// Rotation
//----------------------------------------------------------------
//	Set
	void SetRotation(Quat rotation);
	void SetRotation(float3 rotation);

	void Rotate(Quat rotation);
// Get
	Quat GetRotation() const;
	Quat GetGlobalRotation() const;
//----------------------------------------------------------------
	

// Transform
//----------------------------------------------------------------
	void SetTransform(float4x4 trans);
	void SetIdentity();
//----------------------------------------------------------------


// Matrix
//----------------------------------------------------------------
	float4x4	GetMatrixOGL() const;
	float4x4	GetMatrix() const;

	float4x4	GetLocalMatrix() const;
//----------------------------------------------------------------


	void Inspector();
	void SavePrevTransform(const float4x4 & prevTransformMat);
	void GuizmosOptions();

	void SaveComponent(JSON_Object* parent);

public:
	AABB boundingBox;
	AABB originalBoundingBox;

	bool dragTransform = true;
	bool needRedoOc = false;
private:
	float3 position = float3::zero;
	Quat rotation = Quat::identity;
	float3 scale = float3::one;
};
#endif

