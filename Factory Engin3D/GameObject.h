#pragma once
#include "Globals.h"
#include "Component.h"
#include "Mesh.h"
#include <list>
#include "Transform.h"
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
	GameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr);
	GameObject(TransformInfo* info, GameObject* father = nullptr);
	GameObject(float4x4 transform, GameObject* father = nullptr);

	~GameObject();

	void Update(float dt);

	inline void Delete() { toDelete = true;	}

	Component* GetComponent(ComponentType type);

	void CreateGameObject(TransformInfo* info);

	Component* AddComponent(ComponentType type, ComponentInfo* info);

	int CreateRandomUID();
public:

	bool active = true;

	bool toDelete = false;

	Transform* transform = nullptr;

	list<Component*> components;

	list<GameObject*> childs;
	GameObject* father = nullptr;

	int UID = 0;
	pcg32_random_t rng;
};