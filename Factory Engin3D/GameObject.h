#ifndef __GameObject_H__
#define __GameObject_H__

#include "Globals.h"
#include "Component.h"
#include "Mesh.h"
#include <list>
#include "Transform.h"
#include "Camera.h"

//class Component;
//class Transform;

enum ComponentType;

struct TransformInfo;
struct  ComponentInfo;

class GameObject
{
	friend class ModuleGameObject;

public:
	GameObject(GameObject* father, const char* name = nullptr);
	GameObject(float3 position, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr, const char* name = nullptr);
	
	~GameObject();

	void RemoveChilds();
	void RemoveComponents();

	void Update(float dt);

	inline void Delete() { toDelete = true;	}

	bool SetParent(GameObject* parent);
//Create from json
//----------------------------------------------------------------
	void CreateFromJson(JSON_Object* info, std::vector<uint>& meshesToLoad);
	ComponentInfo* LoadComponentInfo(JSON_Object * info, ComponentType type);
//----------------------------------------------------------------


//Components
//----------------------------------------------------------------
	// Add
	Component* AddComponent(ComponentType type, ComponentInfo* info);
	// Remove
	void RemoveComponent(Component* component);
	// Get
	Component* GetComponent(ComponentType type);
	list<Component*> GetAllComponent(ComponentType type);

	bool HasComponent(ComponentType type);
//----------------------------------------------------------------

//Position
//----------------------------------------------------------------
	// Set
	void SetPos(float3 pos);

	void Move(float3 movement);
	// Get
	float3 GetPos() const;
	float3 GetGlobalPos() const;
//----------------------------------------------------------------

// Scale
//----------------------------------------------------------------
	// Set
	void SetScale(float3 scale);

	void Scale(float3 scale);
	// Get
	float3 GetScale() const;
	float3 GetGlobalScale() const;
//----------------------------------------------------------------


// Rotation
//----------------------------------------------------------------
	// Set
	void SetRotation(Quat rotation);

	void Rotate(Quat rotation);
	// Get
	Quat GetRotation() const;
	Quat GetGlobalRotation() const;
//----------------------------------------------------------------

// Transform
//----------------------------------------------------------------
	void SetTransform(float4x4 transform);
	void SetIdentity();
//----------------------------------------------------------------

// AABB
//----------------------------------------------------------------
	// Set
	void SetABB(AABB aabb);
	// Get
	const AABB* GetAABB() const;
	AABB GetGlobalAABB(AABB localAABB);
	
	float3 GetBBPos();
//----------------------------------------------------------------
	
	
// Matrix
//----------------------------------------------------------------
	float4x4 GetGlobalMatrix() const;
	float4x4 GetLocalMatrix() const;
//----------------------------------------------------------------
	
	
// Active
//----------------------------------------------------------------
	// Set
	void SetActive(bool active);
	// Get
	inline bool GetActive() { return isActive; }
	inline bool* GetActiveReference() { return &isActive; }
//----------------------------------------------------------------

// Static
//----------------------------------------------------------------
	// Set
	void SetObjectStatic(bool isStatic);
	// Get
	inline bool GetObjectStatic() { return isStatic; }
	inline bool* GetStaticReference() { return &isStatic; }
//----------------------------------------------------------------


// UUID
//----------------------------------------------------------------
	// Set
	int CreateRandomUID();
	// Get
	inline const unsigned int GetUID() const { return UID; }
//----------------------------------------------------------------

private:
	void CreateGameObject(TransformInfo* info);

	void RealDelete();

public:
	Transform* transform = nullptr;

	std::list<Component*> components;

	std::list<GameObject*> childs;
	GameObject* father = nullptr;
	uint parentUUID = 0;

	std::string name;

	bool toDelete = false;

private:
	bool isActive = true;

	uint UID = 0;

	bool isStatic = true;
};
#endif // !__GameObject_H__