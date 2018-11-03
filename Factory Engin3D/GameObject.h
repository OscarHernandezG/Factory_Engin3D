#ifndef __GameObject_H__
#define __GameObject_H__

#include "Globals.h"
#include "Component.h"
#include "Mesh.h"
#include <list>
#include "Transform.h"
#include "Camera.h"
#include "pcg-c-0.94/include/pcg_variants.h"

using namespace std;

//class Component;
//class Transform;

enum ComponentType;

struct TransformInfo;
struct  ComponentInfo;

class GameObject
{
public:
	GameObject(GameObject* father, char* name = nullptr);
	GameObject(float3 position, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, char* name = nullptr);
	
	~GameObject();

	void Update(float dt);

	inline void Delete() { toDelete = true;	}

	Component* GetComponent(ComponentType type);
	bool HasComponent(ComponentType type);
	void RemoveComponent(Component* component);
	Component* AddComponent(ComponentType type, ComponentInfo* info);

	void CreateGameObject(TransformInfo* info);

	float3 GetPos();
	Quat GetRotation();

	int CreateRandomUID();
public:

	bool active = true;

	bool toDelete = false;

	Transform* transform = nullptr;

	list<Component*> components;

	list<GameObject*> childs;
	GameObject* father = nullptr;

	string name;

	int UID = 0;
	pcg32_random_t rng;
};
#endif // !__GameObject_H__