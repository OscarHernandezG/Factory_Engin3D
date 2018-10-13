#pragma once
#include "Module.h"

#include "Geometries.h"

class ModuleGeometry : public Module
{
public:
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void Draw3D(bool fill = true, bool wire = false);
	bool CleanUp();

	Mesh* LoadMesh(char* path);

	void UpdateMesh(char * path);

	AABB* LoadBoundingBox(Mesh * mesh);

	void Higher(float& val1, float val2);

	void Lower(float& val1, float val2);

	
	Geometry* LoadPrimitive(PrimitiveTypes type);

	uint LoadTexture(char* path);

	void UpdateTexture(char* path);

public:
	Mesh* currentMesh;
	uint textureID = 0;
	char* droppedFileDir = nullptr;

	float3 max{ 0,0,0 }, min{ 0,0,0 };
};
