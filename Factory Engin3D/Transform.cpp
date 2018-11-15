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
	return position;
}

float3 Transform::GetGlobalPos() const
{
	float3 pos;
	GetMatrix().Decompose(pos, Quat(), float3());

	return pos;
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
	return scale;
}

float3 Transform::GetGlobalScale() const
{
	if (gameObject->father != nullptr)
		return scale.Mul(gameObject->father->GetGlobalScale());

	else return scale;
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
	return rotation;
}

Quat Transform::GetGlobalRotation() const
{
	if (gameObject->father != nullptr)
		return rotation.Mul(gameObject->father->GetGlobalRotation());

	else return rotation;
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
	float4x4 local = GetLocalMatrix();
	if (gameObject->father != nullptr)
		return gameObject->father->GetGlobalMatrix().Mul(local);

	else return local;
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
																			  
	//// Position															  
	////----------------------------------------------------------------------
	JSON_Value* position = json_value_init_object();						  
	JSON_Object* positionObj = json_value_get_object(position);				  
																			  
	json_object_set_value(parent, "Position", position);					  
																			  
	json_object_set_number(positionObj, "X", this->position.x);				  
	json_object_set_number(positionObj, "Y", this->position.y);				  
	json_object_set_number(positionObj, "Z", this->position.z);				  
																			  
	//// Scale																  
	////----------------------------------------------------------------------
	JSON_Value* scale = json_value_init_object();							  
	JSON_Object* scalenObj = json_value_get_object(scale);					  
																			  
	json_object_set_value(parent, "Scale", scale);							  
																			  
	json_object_set_number(scalenObj, "X", this->scale.x);					  
	json_object_set_number(scalenObj, "Y", this->scale.y);					  
	json_object_set_number(scalenObj, "Z", this->scale.z);					  
																			  
	//// Rotation															  
	////----------------------------------------------------------------------
	JSON_Value* rotation = json_value_init_object();						  
	JSON_Object* rotationObj = json_value_get_object(rotation);				  
																			  
	json_object_set_value(parent, "Rotation", rotation);					  
																			  
																			  
	json_object_set_number(rotationObj, "X", this->rotation.x);				  
	json_object_set_number(rotationObj, "Y", this->rotation.y);				  
	json_object_set_number(rotationObj, "Z", this->rotation.z);				  
	json_object_set_number(rotationObj, "W", this->rotation.w);				  
	//------------------------------------------------------------------------
}

