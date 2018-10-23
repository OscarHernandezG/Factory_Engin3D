#include "GameObject.h"


GameObject::GameObject(float3 position, Quat rotation, float3 scale)
{
	TransformInfo info;
	info.position = position;
	info.rotation = rotation;
	info.whichInfo = UsingInfo_TRS;

	CreateGameObject(info);
}

GameObject::GameObject(TransformInfo info)
{
	CreateGameObject(info);
}

GameObject::GameObject(float4x4 transform)
{
	TransformInfo info;
	info.transform = transform;
	info.whichInfo = UsingInfo_TRANSFORM;

	CreateGameObject(info);
}

GameObject::~GameObject()
{

}

void GameObject::CreateGameObject(TransformInfo info)
{
	this->transform = AddComponent(ComponentType_TRANSFORM, info);
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
}