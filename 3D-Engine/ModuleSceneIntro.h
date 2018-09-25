#pragma once
#include "Module.h"
#include "Globals.h"
//#include "Primitive.h"

#include "pcg-c-0.94/include/pcg_variants.h"

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

	bool created = false;

	bool showDemoWindow = false;
	bool exampleWindow = false;
	bool mathGeoLibWindow = false;
	bool randomNumberWindow = false;
	bool aboutWindow = false;
	bool configurationWindow = false;

	bool spheresColliding = false;

	float sphere1Pos = 0;
	float sphere2Pos = 5;

	bool isMinSelected = false;
	bool isMaxSelected = false;

	double randomDoubleNum = -1;

	int num1 = 0;
	int num2 = 0;
	int randomIntNum = -1;

	char* tempText = nullptr;

	string randNumTextInt;
	string randNumTextDouble;


	pcg32_random_t rng;
};
