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
	GameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one);
	GameObject(TransformInfo info);

	GameObject(float4x4 transform);

	~GameObject();

	void CreateGameObject(TransformInfo info);

	Component* AddComponent(ComponentType type, ComponentInfo info);

public:
	Component* transform = nullptr;

	list<Component*> components;
};
