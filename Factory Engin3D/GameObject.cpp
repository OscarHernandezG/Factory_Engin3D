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
		{
			newComponent = (Component*)(((GeometryInfo*)info)->geometry);
			SetABB(((GeometryInfo*)info)->boundingBox);
		}
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

Quat GameObject::GetRotation()
{
	return transform->GetRotation();
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

