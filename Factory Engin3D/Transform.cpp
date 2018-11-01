#include "Transform.h"


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
	return position;
}

Quat Transform::GetRotation() const
{
	return rotation;
}

float4x4 Transform::GetMatrix() const
{
	return float4x4::FromTRS(position, rotation, scale);
}