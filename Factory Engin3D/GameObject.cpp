#include "GameObject.h"
#include "pcg-c-0.94/extras/entropy.h"


GameObject::GameObject(GameObject* father)
{
	TransformInfo* info = new TransformInfo();
	info->position = float3::zero;
	info->rotation = Quat::identity;
	info->scale = float3::one;
	info->whichInfo = UsingInfo_TRS;

	CreateGameObject(info);
	delete info;
}

GameObject::GameObject(float3 position, Quat rotation, float3 scale, GameObject* father) : father(father)
{
	TransformInfo* info = new TransformInfo();
	info->position = position;
	info->rotation = rotation;
	info->scale = scale;
	info->whichInfo = UsingInfo_TRS;

	CreateGameObject(info);
	delete info;
}

GameObject::GameObject(TransformInfo* info, GameObject* father) : father(father)
{
	CreateGameObject(info);
}

GameObject::GameObject(float4x4 transform, GameObject* father) : father(father)
{
	TransformInfo* info = new TransformInfo();
	info->transform = transform;
	info->whichInfo = UsingInfo_TRANSFORM;

	CreateGameObject(info);

	delete info;
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
	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		(*iterator)->Update(dt);
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

void GameObject::RemoveComponent(Component* component)
{
	components.remove(component);
	delete component;
	component = nullptr;
}


void GameObject::CreateGameObject(TransformInfo* info)
{
	pcg32_srandom_r(&rng, 42u, 54u);

	this->transform = (Transform*)AddComponent(ComponentType_TRANSFORM, info);
}

Component* GameObject::AddComponent(ComponentType type, ComponentInfo* info)
{
	Component* newComponent = nullptr;

	info->gameObject = this;

	switch (type)
	{
	case ComponentType_TRANSFORM:
		newComponent = (Component*)new Transform((TransformInfo*)info);
		break;
	case ComponentType_GEOMETRY:
		newComponent = (Component*)(((MeshInfo*)info)->mesh);
		break;
	case ComponentType_CAMERA:
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

float3 GameObject::GetPos()
{
	if (transform != nullptr)
		return transform->GetPos();

	return float3::zero;
}

int GameObject::CreateRandomUID()
{
	UID = pcg32_boundedrand_r(&rng, UINT_MAX);
	return UID;
}

