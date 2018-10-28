#pragma once
#include "GameObject.h"

class GameObject;

enum ComponentType
{
	ComponentType_NONE,

	ComponentType_TRANSFORM,
	ComponentType_GEOMETRY,
	ComponentType_CAMERA,
	ComponentType_TEXTURE,
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

	virtual void Update(float dt) {}

private:
	bool active = true;

	ComponentType type = ComponentType_NONE;

	GameObject* gameObject = nullptr;
};
