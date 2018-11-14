#ifndef __ModuleImGui_H__
#define __ModuleImGui_H__

#include "Module.h"
#include "Globals.h"

#include "imgui-1.65/imgui.h"

#include "parson/parson.h"

#include <string>
#include"MathGeoLib/Math/float2.h"

struct ImVec2;
struct ImVec3;
struct ImVec4;
struct ExampleAppConsole;
class GameObject;

class ModuleImGui : public Module
{
public:
	ModuleImGui(Application* app, bool start_enabled = true);
	~ModuleImGui();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	update_status Save(JSON_Object* object);
	update_status Load(JSON_Object* object);

	bool IsAnyWindowHovered();

	void DrawUI();

	bool CleanUp();

	void CreateExampleWindow();
	void CreateMGLWindow();
	void CreateRandomNumberWindow();

	//Panel About
	void CreateAboutWindow(float2 scale);

	//Panel Configuration
	void CreateConfigWindow(float2 scale);

	void CreateAppHeader();
	void CreateWindowHeader();
	void CreateInputHeader();
	void CreateMeshesHeader();
	void CreateTextureHeader();
	void CreateHardwareHeader();
	void CreateRenderHeader();

	void CreateCPUInfo(ImVec4 color);
	void CreateGPUInfo(ImVec4 color);

	void CheckCaps(std::string* caps);

	//Panel Console
	void CreateConsole(float2 scale);
	void LogConsole(const char* consoleText);

	//Panel Transform
	void CreateTransform(float2 scale);

	//Panel Hierarchy
	void CreateGameObjectHierarchy(float2 scale);
	void CreateGOTreeNode(GameObject * current);

	//PanelTime
	void CreateGameManager(float2 scale);

	//Panel Resources
	void CreateAssetsWindow(float2 scale);
	void TreeAssets(const char * path);

	void SetWindowDim(float2 & pos, float2 & size, float2 & scale, bool gameWindow = false);

	update_status CreateMainMenuBar();

	void CreateMenu();
	bool CreateOptions();
	void CheckShortCuts();
	void CreateDebugMenu();

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
	bool hierarchyWindow = true;
	bool exitOption = false;


	bool spheresColliding = false;

	float sphere1Pos = 0;
	float sphere2Pos = 5;

	bool isMinSelected = false;
	bool isMaxSelected = false;

	double randomDoubleNum = -1;

	int randNum1 = 0;
	int randNum2 = 0;
	int randomIntNum = -1;

	char* tempText = nullptr;

	std::string randNumTextInt;
	std::string randNumTextDouble;
	float brightnessPos = 1.0f;
	int widthPos = SCREEN_WIDTH;
	int heightPos = SCREEN_HEIGHT;

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color;

	float f = 0.0f;
	int counter = 0;

	//console
	ImGuiTextBuffer textBuff;
	bool canScroll = false;

	std::vector<float> ramLog;

	int resize = 0;

	bool warningDialoge = false;

	bool dragTransform = false;

	//--------------------------
	// Window position
	float2 aboutPos = float2::zero;
	float2 configurationPos = float2::zero;
	float2 transformPos = float2::zero;
	float2 consolePos = float2::zero;
	float2 scenePos = float2::zero;
	float2 playPos = float2::zero;
	float2 playCountPos = float2::zero;
	// Window sizes
	float2 aboutSize = float2::zero;
	float2 configurationSize = float2::zero;
	float2 transformSize = float2::zero;
	float2 consoleSize = float2::zero;
	float2 sceneSize = float2::zero;
	float2 playSize = float2::zero;
	float2 playCountSize = float2::zero;
	//--------------------------
};
#endif // !__ModuleImGui_H__