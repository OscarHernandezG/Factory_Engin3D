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

void Transform::Update(float dt)
{

	UpdateBoundingBox();
}

void Transform::UpdateBoundingBox()
{
	OBB obb = originalBoundingBox.ToOBB();
	obb.Transform(GetMatrix());

	boundingBox = obb.MinimalEnclosingAABB();
}

void Transform::SetPos(float x, float y, float z)
{
	SetPos(float3(x, y, z));
}

void Transform::SetPos(float3 position)
{
	float3 pos = position;

	if (gameObject->father != nullptr)
		pos = pos - gameObject->father->GetPos();

	this->position = pos;
}

void Transform::SetRotation(Quat rotation)
{
	this->rotation = rotation;
}

void Transform::SetRotation(float3 rotation)
{
	this->rotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
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

float3 Transform::GetScale() const
{
	if (gameObject->father != nullptr)
		return position + gameObject->father->GetScale();

	else return scale;
}

Quat Transform::GetRotation() const
{
	if (gameObject->father != nullptr)
		return rotation.Mul(gameObject->father->GetRotation());

	else return rotation;
}

float3 Transform::GetLocalPos() const
{
	return position;
}

Quat Transform::GetLocalRotation() const
{
	return rotation;
}


float4x4 Transform::GetMatrix() const
{
	if (scale.y == 2.0f)
		bool pase = 0;

	return float4x4::FromTRS(position, rotation, scale).Transposed();
}