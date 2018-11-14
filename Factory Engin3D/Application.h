#ifndef __Application_H__
#define __Application_H__

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
#include "ModuleGeometry.h"
#include "ModuleImporter.h"
#include "ModuleGameObject.h"
#include "ModulePicking.h"
#include "ModuleTime.h"
#include "ModuleResources.h"

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
	ModuleGameObject* gameObject;
	ModuleImporter* importer;
	ModulePicking* picking;
	ModuleTime* time;
	ModuleResources* resources;

private:
	std::list<Module*> list_modules;

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

	bool toCap = true;
	bool canSave = false;
	bool canLoad = false;
	bool isMaximized = false;
	int capFrames = 60;
	std::string aplicationName;
};

extern Application* App;
#endif // !__Application_H__
