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

	Mesh LoadMesh(char* path);

	Primitive* LoadPrimitive(PrimitiveTypes type);

	uint LoadTexture(char* path);

public:
	Mesh warrior;

	char* droppedFileDir = nullptr;
};
