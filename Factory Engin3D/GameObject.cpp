#include "GameObject.h"
#include "pcg-c-0.94/extras/entropy.h"


GameObject::GameObject(GameObject* father, const char * name)
{
	TransformInfo* info = new TransformInfo();
	info->position = float3::zero;
	info->rotation = Quat::identity;
	info->scale = float3::one;
	info->whichInfo = UsingInfo_TRS;

	CreateGameObject(info);
	delete info;

	this->father = father;

	if (name != nullptr)
		this->name = name;
	else this->name = "noName";
}

GameObject::GameObject(float3 position, Quat rotation, float3 scale, GameObject* father, const char* name)
{
	TransformInfo* info = new TransformInfo();
	info->position = position;
	info->rotation = rotation;
	info->scale = scale;
	info->whichInfo = UsingInfo_TRS;

	CreateGameObject(info);
	delete info;

	this->father = father;

	if (name != nullptr)
		this->name = name;
	else this->name = "noName";
}


GameObject::~GameObject()
{
	transform = nullptr;

	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		delete (*iterator);
	}

	components.clear();

	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		delete (*iterator);
	}

	childs.clear();
}

void GameObject::Update(float dt)
{
	if (isActive)
	{
		for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
		{
			(*iterator)->Update(dt);
		}

		for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
		{
			(*iterator)->Update(dt);
		}
	}
}

Component* GameObject::GetComponent(ComponentType type)
{
	Component* component = nullptr;

	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->type == type)
		{
			component = (*iterator);
			break;
		}
	}

	return component;
}

list<Component*> GameObject::GetAllComponent(ComponentType type)
{
	list<Component*> component;

	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->type == type)
		{
			components.push_back(*iterator);
		}
	}

	return component;
}

bool GameObject::HasComponent(ComponentType type)
{
	bool ret = false;
	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		if ((*iterator)->type == type)
		{
			ret = true;
			break;
		}
	}
	return ret;
}

void GameObject::RemoveComponent(Component* component)
{
	if (component != nullptr)
	{
		components.remove(component);
		delete component;
		component = nullptr;
	}
}


void GameObject::CreateGameObject(TransformInfo* info)
{
	pcg32_srandom_r(&rng, 42u, 54u);

	UID = pcg32_boundedrand_r(&rng, 2147483647);

	this->transform = (Transform*)AddComponent(ComponentType_TRANSFORM, info);
}

Component* GameObject::AddComponent(ComponentType type, ComponentInfo* info)
{
	Component* newComponent = nullptr;

	if (info)
		info->gameObject = this;

	switch (type)
	{
	case ComponentType_TRANSFORM:
		if (info)
			newComponent = (Component*)new Transform((TransformInfo*)info);
		break;
	case ComponentType_GEOMETRY:
		if (info)
			newComponent = (Component*)(((GeometryInfo*)info)->geometry);
		break;
	case ComponentType_CAMERA:
		newComponent = (Component*)new Camera(this);
		break;
	case ComponentType_TEXTURE:
		break;
	case ComponentType_LIGHT:
		break;
	default:
		break;
	}

	if (newComponent != nullptr)
	{
		newComponent->type = type;
		newComponent->isActive = true;
		newComponent->gameObject = this;
		components.push_back(newComponent);
	}
	return newComponent;
}

float3 GameObject::GetPos() const
{
	if (transform != nullptr)
		return transform->GetPos();

	return float3::zero;
}

float3 GameObject::GetScale() const
{
	if (transform != nullptr)
		return transform->GetScale();

	return float3::one;
}

float4x4 GameObject::GetGlobalMatrix() const
{
	float4x4 mat = float4x4::identity;
	if (transform)
	{
		mat = transform->GetGlobalMatrix();
	}

	return mat;
}

Quat GameObject::GetRotation() const
{
	return transform->GetRotation().Normalized();
}

void GameObject::SetTransform(float4x4 trans)
{
	if (transform)
	{
		float3 pos, scale;
		Quat rot;
		trans.Decompose(pos, rot, scale);

		SetPos(pos);
		SetScale(scale);
		SetRotation(rot);
	}
}

void GameObject::SetPos(float3 pos)
{
	float3 movement = float3::zero;
	if (transform)
	{
		movement = pos - transform->GetPos();
	}
	Move(movement);

}

void GameObject::Move(float3 movement)
{
	if (transform)
		transform->Move(movement);

	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		(*iterator)->Move(movement);
	}
}

void GameObject::SetScale(float3 scale)
{
	float3 scaleVariation;
	float3 scaleGO = GetScale();

	scaleVariation.x = scale.x / scaleGO.x;
	scaleVariation.y = scale.y / scaleGO.y;
	scaleVariation.z = scale.z / scaleGO.z;

	Scale(scaleVariation);
}

void GameObject::Scale(float3 scale)
{
	if (transform)
		transform->Scale(scale);

	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		(*iterator)->Scale(scale);
		(*iterator)->SetPos(scale.Mul((*iterator)->transform->GetLocalPos()));
	}
}

void GameObject::SetRotation(Quat rotation)
{
	Quat rotate = rotation.Mul(GetRotation().Inverted()).Normalized();

	Rotate(rotate);
}

void GameObject::Rotate(Quat rotation)
{
	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		float3 originalPosition = (*iterator)->GetPos() - GetPos();
		float3 newPosition = originalPosition;
		newPosition = rotation.Transform(newPosition);

		(*iterator)->SetPos(newPosition);
		(*iterator)->Rotate(rotation);
	}

	if (transform)
		transform->Rotate(rotation);
}

void GameObject::SetIdentity()
{
	if (transform)
	{
		SetPos(float3::zero);
		SetRotation(Quat::identity);
		SetScale(float3::one);

		transform->UpdateBoundingBox();
	}
}

const AABB* GameObject::GetAABB() const
{
	return &transform->boundingBox;
}

float3 GameObject::GetBBPos() const
{
	float3 distance{ 0,0,0 };
	if (transform)
	{
		float3 size = transform->boundingBox.Size();

		float3 reScale = GetScale();
		distance.x = (size.x / 2) / math::Tan((0.3333333 * reScale.x));
		distance.y = (size.y / 2) / math::Tan((0.3333333 * reScale.y));
		distance.z = (size.z / 2) / math::Tan((0.3333333 * reScale.z));
	}
	return distance + GetPos();
}

void GameObject::SetABB(AABB aabb)
{
	if (transform)
	{
		transform->originalBoundingBox = aabb;

		transform->UpdateBoundingBox();
	}
}

void GameObject::SetActive(bool active)
{
	for (list<GameObject*>::iterator iterator = childs.begin(); iterator != childs.end(); ++iterator)
	{
		(*iterator)->SetActive(active);
	}
	isActive = active;
}

int GameObject::CreateRandomUID()
{
	UID = pcg32_boundedrand_r(&rng, UINT_MAX);
	return UID;
}

