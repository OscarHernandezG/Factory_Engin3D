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

	void Draw3D(bool fill = true, bool wire = false);
	bool CleanUp();

	Mesh* LoadMesh(char* path);

	void UpdateMesh(char * path);

	AABB * LoadBoundingBox(Mesh * mesh);

	float3 CalcBBPos(math::AABB* boundingBox);

	float3 GetBBPos();

	float3 GetCurrentMeshPivot();

	void Higher(float & val1, float val2);

	void Lower(float & val1, float val2);

	Geometry* LoadPrimitive(PrimitiveTypes type);

	uint LoadTexture(char* path);

	void UpdateTexture(char* path);

public:
	Mesh* currentMesh = nullptr;

	uint textureID = 0;
	uint numFaces = 0;

	char* droppedFileDir = nullptr;

	AABB* currentMeshBB = nullptr;
};
