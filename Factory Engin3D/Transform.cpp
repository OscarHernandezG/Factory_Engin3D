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

//Position
//-------------------------------------------------------------------------
//	Set
void Transform::SetPos(float x, float y, float z)
{
	SetPos(float3(x, y, z));
}

void Transform::SetPos(float3 position)
{
	this->position = position;
}


void Transform::Move(float3 position)
{
	this->position = this->position.Add(position);
}
//	Get
float3 Transform::GetPos() const
{
	if (gameObject)
		if (gameObject->father)
			return position + gameObject->father->GetPos();

		else return position;
}

float3 Transform::GetLocalPos() const
{
	return position;
}
//-------------------------------------------------------------------------



// Scale
//-------------------------------------------------------------------------
//	Set
void Transform::SetScale(float x, float y, float z)
{
	scale = float3(x, y, z);
}

void Transform::SetScale(float3 scale)
{
	this->scale = scale;
}


void Transform::Scale(float3 scale)
{
	this->scale = this->scale.Mul(scale);
}
//	Get
float3 Transform::GetScale() const
{
	if (gameObject->father != nullptr)
		return scale.Mul(gameObject->father->GetScale());

	else return scale;
}

float3 Transform::GetLocalScale() const
{
	return scale;
}
//-------------------------------------------------------------------------



// Rotation
//-------------------------------------------------------------------------
//	Set
void Transform::SetRotation(Quat rotation)
{
	this->rotation = rotation;
}

void Transform::SetRotation(float3 rotation)
{
	this->rotation = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
}

void Transform::Rotate(Quat rotation)
{
	this->rotation = rotation.Mul(this->rotation).Normalized();
}
//	Get
Quat Transform::GetRotation() const
{
	if (gameObject->father != nullptr)
		return rotation.Mul(gameObject->father->GetRotation());

	else return rotation;
}

Quat Transform::GetLocalRotation() const
{
	return rotation;
}
//-------------------------------------------------------------------------



// Transform
//-------------------------------------------------------------------------
void Transform::SetTransform(float4x4 trans)
{
	trans.Decompose(position, rotation, scale);
}

void Transform::SetIdentity()
{
	position = float3::zero;
	rotation = Quat::identity;
	scale = float3::one;
}
//-------------------------------------------------------------------------



// Matrix
//-------------------------------------------------------------------------
float4x4 Transform::GetMatrixOGL() const
{
	return GetMatrix().Transposed();
}

float4x4 Transform::GetMatrix() const
{
	return float4x4::FromTRS(GetPos(), GetRotation(), GetScale());
}

float4x4 Transform::GetLocalMatrix() const
{
	return float4x4::FromTRS(position, rotation, scale);
}
//-------------------------------------------------------------------------



void Transform::SaveComponent(JSON_Object * parent)
{

	json_object_set_number(parent, "Type", this->type);

	json_object_set_number(parent, "UUID", GetUUID());

	json_object_set_number(parent, "Time Created", GetTime());

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

