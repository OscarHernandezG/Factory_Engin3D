#include "Transform.h"

#include "GameObject.h"

Transform::Transform(TransformInfo* info) : Component(info->gameObject, ComponentType_TRANSFORM)
{
	switch (info->whichInfo)
	{
	case UsingInfo_TRS:
		position = info->position;
		rotation = info->rotation;
		scale = info->scale;
		break;
	default:
		break;
	}

}

Transform::Transform(GameObject * gameObject) : Component(gameObject, ComponentType_TRANSFORM)
{
	SetIdentity();
}

void Transform::SetPos(float x, float y, float z)
{
	position = float3(x, y, z);
}

void Transform::SetPos(float3 position)
{
	this->position = position;
}

void Transform::SetRotation(Quat rotation)
{
	this->rotation = rotation;
}

// ------------------------------------------------------------
void Transform::Rotate(Quat rotation)
{
	this->rotation = this->rotation * rotation;
}

// ------------------------------------------------------------
void Transform::SetScale(float x, float y, float z)
{
	scale = float3(x, y, z);
}

void Transform::SetIdentity()
{
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;
}

float3 Transform::GetPos() const
{
	if (gameObject->father != nullptr)
		return position + gameObject->father->GetPos();

	else return position;
}

Quat Transform::GetRotation() const
{
	if (gameObject->father != nullptr)
		return rotation.Mul(gameObject->father->GetRotation());

	else return rotation;
}

inline float3 Transform::GetLocalPos() const
{
	return position;
}

inline Quat Transform::GetLocalRotation() const
{
	return rotation;
}


float4x4 Transform::GetMatrix() const
{
	float4x4 mat = float4x4::FromTRS(position, rotation, scale).Transposed();
		return mat;
}