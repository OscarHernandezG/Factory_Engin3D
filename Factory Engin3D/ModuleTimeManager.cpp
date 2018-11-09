#include "Globals.h"
#include "Application.h"
#include "ModuleTimeManager.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>

ModuleTimeManager::ModuleTimeManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleTimeManager::~ModuleTimeManager()
{
}

// Called before render is available
bool ModuleTimeManager::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleTimeManager::Start()
{
	dtTimer.Start();
	return true;
}

// Called before quitting
bool ModuleTimeManager::CleanUp()
{
	return true;
}

update_status ModuleTimeManager::PreUpdate()
{
	//Calculate dtReal
	dtReal = (float)dtTimer.Read() / 1000.0f;
	dtTimer.Start();

	//frames since start
	frameCount++;

	//FPS app window
	fpsLog.push_back(1 / dtReal);

	if (fpsLog.size() > 75)
	{
		fpsLog.erase(fpsLog.begin());
	}
	return UPDATE_CONTINUE;
}

update_status ModuleTimeManager::Update()
{
	return UPDATE_CONTINUE;
}

update_status ModuleTimeManager::PostUpdate()
{

	msLog.push_back(dtReal * 1000);

	if (msLog.size() > 75)
	{
		msLog.erase(msLog.begin());
	}

	return UPDATE_CONTINUE;
}


uint ModuleTimeManager::GetFrameCount() const
{
	return frameCount;
}
float ModuleTimeManager::Getdt() const
{
	return dtReal;
}
float ModuleTimeManager::GetStartGame() const
{
	return startsTime;
}
float ModuleTimeManager::GetdtGame() const
{
	return dtGame;
}
float ModuleTimeManager::GetdtGameScale() const
{
	return dtGameScale;
}
float ModuleTimeManager::GetGameTimer() const
{
	return gameTimer;
}


update_status ModuleTimeManager::Save(JSON_Object* object)
{

	return UPDATE_CONTINUE;
}

update_status ModuleTimeManager::Load(JSON_Object * object)
{

	return UPDATE_CONTINUE;
}
