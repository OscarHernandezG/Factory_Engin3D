#pragma once
#include "Module.h"
#include "Globals.h"
#include "Mesh.h"

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void Draw3D(bool fill = true, bool wire = false);
	bool CleanUp();

	Mesh LoadMesh();

public:
};
