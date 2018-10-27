#include "Transform.h"


Transform::Transform(TransformInfo* info) : Component(info->gameObject, ComponentType_TRANSFORM)
{
	switch (info->whichInfo)
	{
	case UsingInfo_TRS:
		transform.FromTRS(info->position, info->rotation, info->scale);
		break;
	case UsingInfo_TRANSFORM:
		transform = info->transform;
	default:
		break;
	}

}

void Transform::SetPos(float x, float y, float z)
{
	transform[3][0] = x;
	transform[3][1] = y;
	transform[3][2] = z;
}

// ------------------------------------------------------------
void Transform::SetRotation(float angle, const float3 &u)
{
	transform = float4x4::RotateAxisAngle(u, angle) * transform;
}

// ------------------------------------------------------------
void Transform::SetScale(float x, float y, float z)
{
	float4x4 initialScale = float4x4::identity;
	//SetPos
	initialScale[3][0] = transform[3][0];
	initialScale[3][1] = transform[3][1];
	initialScale[3][2] = transform[3][2];
	transform = float4x4::Scale(x, y, z).ToFloat4x4() * initialScale;
}

void Transform::SetIdentity()
{
	transform = float4x4::identity;
}

float3 Transform::GetPos() const
{
	return { transform[3][0], transform[3][1], transform[3][2] };
}