#pragma once
#include "Module.h"
#include "Globals.h"
//#include "Primitive.h"

#include "imgui.h"

#include "pcg-c-0.94/include/pcg_variants.h"

#define MAX_SNAKE 8

struct PhysBody3D;
struct ImVec2;
struct ImVec3;
struct ImVec4;
struct ExampleAppConsole;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void Draw3D();

	bool CleanUp();

public:

	int widthPos = SCREEN_WIDTH;
	int heightPos = SCREEN_HEIGHT;
};
