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
	GameObject(GameObject* father, const char* name = nullptr);
	GameObject(float3 position, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, const char* name = nullptr);
	
	~GameObject();

	void Update(float dt);

	inline void Delete() { toDelete = true;	}

	Component* GetComponent(ComponentType type);
	list<Component*> GetAllComponent(ComponentType type);
	bool HasComponent(ComponentType type);
	void RemoveComponent(Component* component);
	Component* AddComponent(ComponentType type, ComponentInfo* info);


	float3 GetPos() const;
	float3 GetScale() const;
	Quat GetRotation() const;
	float4x4 GetGlobalMatrix() const;

	void SetTransform(float4x4 transform);

	void ForceTransform(float4x4 trans);

	void SetPos(float3 pos);
	void Move(float3 movement);
	void SetScale(float3 scale);
	void Scale(float3 scale);
	void SetRotation(Quat rotation);

	void Rotate(Quat rotation);

	void SetIdentity();

	const AABB* GetAABB() const;

	float3 GetBBPos() const;

	void SetABB(AABB aabb);

	void SetActive(bool active);
	void SetObjectStatic(bool isStatic);

	inline bool GetActive() { return isActive; }
	inline bool GetObjectStatic() { return isStatic; }

	inline bool* GetActiveReference() { return &isActive; }
	inline bool* GetStaticReference() { return &isStatic; }



	int CreateRandomUID();

private:
	void CreateGameObject(TransformInfo* info);


public:
	Transform* transform = nullptr;

	list<Component*> components;

	list<GameObject*> childs;
	GameObject* father = nullptr;

	string name;

	bool toDelete = false;

private:
	bool isActive = true;
	bool isStatic = true;

	int UID = 0;
	pcg32_random_t rng;

};
#endif // !__GameObject_H__