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

void Transform::Move(float3 position)
{
	this->position = this->position.Add(position);
}

void Transform::SetTransform(float4x4 trans)
{
	trans.Decompose(position, rotation, scale);
}

void Transform::Scale(float3 scale)
{
	this->scale = this->scale.Mul(scale);
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
	float3 angles = this->rotation.ToEulerXYZ();
	this->rotation = rotation.Mul(this->rotation).Normalized();
}

// ------------------------------------------------------------
void Transform::SetScale(float x, float y, float z)
{
	scale = float3(x, y, z);
}

void Transform::SetScale(float3 scale)
{
	this->scale = scale;
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
		return scale.Mul(gameObject->father->GetScale());

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


const float4x4 Transform::GetMatrixOGL() const
{
	return float4x4::FromTRS(position, rotation, scale).Transposed();
}

const float4x4 Transform::GetMatrix() const
{
	return float4x4::FromTRS(position, rotation, scale);
}

const float4x4 Transform::GetGlobalMatrix() const
{
	return float4x4::FromTRS(GetPos(), GetRotation(), GetScale());
}

void Transform::SaveComponent(JSON_Object * parent)
{

	json_object_set_number(parent, "Type", this->type);

	json_object_set_number(parent, "UUID", GetUUID());


	// Position
	//------------------------------------------------------------------------
	SaveNumberArray(parent, "Position", GetPos().ptr(), 3);

	// Scale
	//------------------------------------------------------------------------
	SaveNumberArray(parent, "Scale", GetScale().ptr(), 3);

	// Rotation
	//------------------------------------------------------------------------
	SaveNumberArray(parent, "Rotation", GetRotation().ptr(), 4);

	// Bounding box
	//------------------------------------------------------------------------
	JSON_Value* boundingBoxV = json_value_init_object();
	JSON_Object* boundingBoxObj = json_value_get_object(boundingBoxV);

	json_object_set_value(parent, "Bounding Box", boundingBoxV);
	{
		// Min point
		SaveNumberArray(boundingBoxObj, "Min", boundingBox.minPoint.ptr(), 3);

		// Max point
		SaveNumberArray(boundingBoxObj, "Max", boundingBox.maxPoint.ptr(), 3);
	}
	// Original Bounding box
	//------------------------------------------------------------------------
	JSON_Value* originalBB = json_value_init_object();
	JSON_Object* originalBBObj = json_value_get_object(originalBB);

	json_object_set_value(parent, "Original Bounding Box", originalBB);
	{
		// Min point
		SaveNumberArray(originalBBObj, "Min", originalBoundingBox.minPoint.ptr(), 3);

		// Max point
		SaveNumberArray(originalBBObj, "Max", originalBoundingBox.maxPoint.ptr(), 3);
	}
}

