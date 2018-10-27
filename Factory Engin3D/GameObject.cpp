#include "GameObject.h"
#include "pcg-c-0.94/extras/entropy.h"


GameObject::GameObject(float3 position, Quat rotation, float3 scale, GameObject* father) : father(father)
{
	TransformInfo* info = new TransformInfo();
	info->position = position;
	info->rotation = rotation;
	info->whichInfo = UsingInfo_TRS;

	pcg32_srandom_r(&rng, 42u, 54u);

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
}

void GameObject::Update()
{
	for (list<Component*>::iterator iterator = components.begin(); iterator != components.end(); ++iterator)
	{
		(*iterator)->Update();
	}
}

void GameObject::CreateGameObject(TransformInfo* info)
{
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

	components.push_back(newComponent);

	return newComponent;
}

int GameObject::CreateRandomUID()
{
	UID = pcg32_boundedrand_r(&rng, UINT_MAX);
	return UID;
}

