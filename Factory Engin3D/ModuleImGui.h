#pragma once
#include "Module.h"
#include "Globals.h"

#include "imgui.h"

#include "parson/parson.h"
#include "pcg-c-0.94/include/pcg_variants.h"

using namespace std;

struct PhysBody3D;
struct ImVec2;
struct ImVec3;
struct ImVec4;
struct ExampleAppConsole;

class ModuleImGui : public Module
{
public:
	ModuleImGui(Application* app, bool start_enabled = true);
	~ModuleImGui();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status Save(JSON_Object* object);
	update_status Load(JSON_Object* object);

	bool IsAnyWindowHovered();

	void DrawUI();


	bool CleanUp();

	void CreateExampleWindow();
	void CreateMGLWindow();
	void CreateRandomNumberWindow();
	void CreateAboutWindow();
	void CreateConfigWindow();
	void CreateConsole();
	void CreateTransform();


	update_status CreateMainMenuBar();

	void CreateMenu();
	bool CreateOptions();
	void CheckShortCuts();

	void CreateAppHeader();
	void CreateWindowHeader();
	void CreateInputHeader();
	void CreateTextureHeader();
	void CreateHardwareHeader();
	void CreateRenderHeader();

	void CreateCPUInfo(ImVec4 color);
	void CreateGPUInfo(ImVec4 color);

	void CheckCaps(string* caps);

	void LogConsole(const char* consoleText);

	void ResizeImGui(float2 scale);

public:
	bool created = false;

	bool showDemoWindow = false;
	bool exampleWindow = false;
	bool mathGeoLibWindow = false;
	bool randomNumberWindow = false;
	bool aboutWindow = false;
	bool configurationWindow = false;
	bool consoleWindow = false;
	bool transformWindow = false;
	bool exitOption = false;


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
	float brightnessPos = 1.0f;
	int widthPos = SCREEN_WIDTH;
	int heightPos = SCREEN_HEIGHT;

	pcg32_random_t rng;

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color;

	float f = 0.0f;
	int counter = 0;

	//console
	ImGuiTextBuffer textBuff;
	bool canScroll = false;

	vector<float> ramLog;

	int resize = 0;

};
