#pragma once
#include "GameObject.h"

enum ComponentType
{
	ComponentType_NONE,
	ComponentType_TRANSFORM,
	ComponentType_LIGHT
};

struct ComponentInfo
{
	GameObject* gameObject = nullptr;
};

class Component
{
public:
	Component(GameObject* gameObject) : gameObject(gameObject), type(ComponentType_NONE){};
	Component(GameObject* gameObject, ComponentType type) : gameObject(gameObject), type(ComponentType_NONE) {};

	~Component() {};

private:
	bool active = true;

	ComponentType type = ComponentType_NONE;

	GameObject* gameObject = nullptr;
};
