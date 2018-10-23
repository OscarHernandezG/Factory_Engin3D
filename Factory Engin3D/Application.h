#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleImGui.h"
#include "ModuleGeometryManager.h"
#include "Importer.h"

using namespace std;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* sceneIntro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleImGui* gui;
	ModuleGeometry* geometry;
	ModuleImporter* importer;

private:

	Timer	ms_timer;
	float	dt;
	list<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	update_status Save(JSON_Object * object);
	update_status Load(JSON_Object * object);
	void InitialLoad();

	void LogString(const char* texLog) const;

	void ChangeAppName(char* newName);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

public:

	vector<float> fpsLog;
	vector<float> msLog;

	bool toCap = true;
	bool canSave = false;
	bool canLoad = false;
	bool isMaximized = false;
	int capFrames = 60;
	std::string aplicationName;
};

extern Application* App;
