#pragma once

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

	inline void SetActive(bool isActive) { this->isActive = isActive; }

public:
	ComponentType type = ComponentType_NONE;

	bool isActive = true;

	GameObject* gameObject = nullptr;
};