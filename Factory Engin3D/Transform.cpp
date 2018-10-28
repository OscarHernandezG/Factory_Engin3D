#include "Transform.h"


Transform::Transform(TransformInfo* info) : Component(info->gameObject, ComponentType_TRANSFORM)
{
	switch (info->whichInfo)
	{
	case UsingInfo_TRS:
		matrix.FromTRS(info->position, info->rotation, info->scale);
		break;
	case UsingInfo_TRANSFORM:
		matrix = info->transform;
	default:
		break;
	}

}

Transform::Transform(GameObject * gameObject) : Component(gameObject, ComponentType_TRANSFORM)
{
	matrix = float4x4::identity;
}

Transform::Transform(GameObject * gameObject, float4x4 transform) : Component(gameObject, ComponentType_TRANSFORM)
{
	this->matrix = transform;
}

void Transform::SetPos(float x, float y, float z)
{
	matrix[3][0] = x;
	matrix[3][1] = y;
	matrix[3][2] = z;
}

// ------------------------------------------------------------
void Transform::SetRotation(float angle, const float3 &u)
{
	matrix = float4x4::RotateAxisAngle(u, angle) * matrix;
}

// ------------------------------------------------------------
void Transform::SetScale(float x, float y, float z)
{
	float4x4 initialScale = float4x4::identity;
	//SetPos
	initialScale[3][0] = matrix[3][0];
	initialScale[3][1] = matrix[3][1];
	initialScale[3][2] = matrix[3][2];
	matrix = float4x4::Scale(x, y, z).ToFloat4x4() * initialScale;
}

void Transform::SetIdentity()
{
	matrix = float4x4::identity;
}

float3 Transform::GetPos() const
{
	return { matrix[3][0], matrix[3][1], matrix[3][2] };
}