#pragma once
#include "Globals.h"
#include "Mesh.h"
#include "Component.h"
#include <list>
#include "Transform.h"
using namespace std;


class GameObject
{
public:
	GameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr);
	GameObject(TransformInfo* info, GameObject* father = nullptr);


	GameObject(float4x4 transform, GameObject* father = nullptr);

	~GameObject();

	void CreateGameObject(TransformInfo* info);

	Component* AddComponent(ComponentType type, ComponentInfo* info);

public:

	bool active = true;

	Transform* transform = nullptr;

	list<Component*> components;

	list<GameObject*> childs;
	GameObject* father = nullptr;
};