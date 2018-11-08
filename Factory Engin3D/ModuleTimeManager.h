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

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	update_status Save(JSON_Object * object);
	update_status Load(JSON_Object * object);

public:

	uint frameCount;			//app graphics frames since game start

	float dtReal;				//last frame time expressed in seconds(Real)
	float dtRealGameStart;		//seconds since game start(Real)
	
	float dtGame;				//last frame time expressed in seconds(Game)
	float dtGameScale;			//scale at which time is passing(Game)
	float dtGameStart;			//seconds since game start(Game)

	bool isPaused;

	vector<float> fpsLog;
	vector<float> msLog;

private:
	Timer	ms_timer;

};

#endif // __ModuleTimeManager_H__