#include "GameObject.h"


GameObject::GameObject(float3 position, Quat rotation, float3 scale, GameObject* father) : father(father)
{
	TransformInfo* info = new TransformInfo();
	info->position = position;
	info->rotation = rotation;
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
	case ComponentType_LIGHT:
		break;
	default:
		break;
	}

	return newComponent;
}