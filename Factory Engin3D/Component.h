#ifndef __Component_H__
#define __Component_H__
#include "parson/parson.h"
#include <time.h>
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
	
	unsigned int UUID = 0;
};

class Component
{
public:
	Component(GameObject* gameObject) : gameObject(gameObject), type(ComponentType_NONE) { timeCreated = time(NULL); }
	Component(GameObject* gameObject, ComponentType type) : gameObject(gameObject), type(ComponentType_NONE) {}

	~Component() {}

	virtual void Update(float dt) {}

	inline void SetActive(bool isActive) { this->isActive = isActive; }

	virtual void SaveComponent(JSON_Object* parent) {}

	unsigned int GetUUID() const { return UUID; }
	const void SetUUID(unsigned int newUUID) { UUID = newUUID; }	// This can only be used in load stage

	__int64 GetTime() const { return timeCreated; }
	const void SetTime(__int64 newTime) { timeCreated = newTime; }	// This can only be used in load stage

public:
	ComponentType type = ComponentType_NONE;

	bool isActive = true;

	GameObject* gameObject = nullptr;

private:

	unsigned int UUID = 0;

	__int64 timeCreated = 0;
};
#endif