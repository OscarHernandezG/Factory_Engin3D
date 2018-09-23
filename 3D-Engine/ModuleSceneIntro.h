#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"

#define MAX_SNAKE 8

struct PhysBody3D;
struct ImVec2;
struct ImVec3;
struct ImVec4;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
};
