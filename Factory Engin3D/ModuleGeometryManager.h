#pragma once
#include "Module.h"

#include "Geometries.h"

class ModuleGeometry : public Module
{
public:
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Start();
	void LoadDefaultScene();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void Draw3D(bool fill = true, bool wire = false) const;
	bool CleanUp();

	Mesh* LoadMesh(char* path);

	void UpdateMesh(char * path);

	AABB * LoadBoundingBox(Mesh * mesh);

	float3 CalcBBPos(math::AABB* boundingBox) const;

	float3 GetBBPos() const;

	float3 GetCurrentMeshPivot() const;

	void Higher(float& val1, float val2);

	void Lower(float& val1, float val2);

	//Geometry* LoadPrimitive(PrimitiveTypes type);

	uint LoadTexture(char* path) const;

	void UpdateTexture(char* path);

public:
	Mesh* currentMesh = nullptr;

	uint textureID = 0;
	uint numFaces = 0;

	char* droppedFileDir = nullptr;

	AABB* currentMeshBB = nullptr;
};


class ModuleGameObjectManager : public Module
{
public:
	ModuleGameObjectManager(Application* app, bool start_enabled = true);
	~ModuleGameObjectManager();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

	GameObject* CreateGameObject(float3 position = float3::zero, Quat rotation = Quat::identity, float3 scale = float3::one, GameObject* father = nullptr);
	GameObject* CreateGameObject(TransformInfo* info, GameObject* father = nullptr);
	GameObject* CreateGameObject(float4x4 transform, GameObject* father = nullptr);

public:
	GameObject* root;

	list<GameObject*> gameObjects;

	
};
