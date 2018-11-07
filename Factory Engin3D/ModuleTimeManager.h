#ifndef __ModuleTimeManager_H__
#define __ModuleTimeManager_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleTimeManager : public Module
{
public:

	ModuleTimeManager(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleTimeManager();

	bool Init();
	bool Start();
	bool CleanUp();

	update_status Save(JSON_Object * object);
	update_status Load(JSON_Object * object);

};

#endif // __ModuleTimeManager_H__