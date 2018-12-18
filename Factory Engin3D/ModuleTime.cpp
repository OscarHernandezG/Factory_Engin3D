#include "Globals.h"
#include "Application.h"
#include "ModuleTime.h"

#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>

ModuleTime::ModuleTime(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleTime::~ModuleTime()
{
}

// Called before render is available
bool ModuleTime::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleTime::Start()
{
	dtTimer.Start();
	return true;
}

// Called before quitting
bool ModuleTime::CleanUp()
{
	return true;
}

update_status ModuleTime::PreUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	//Calculate dtReal
	dtReal = (float)dtTimer.Read() / 1000.0f;

	//Calculate dtGame
	if (gameState == GameState_PLAYING || gameState == GameState_TICK)
	{
		dtGame = (float)dtTimer.Read() / 1000.0f;
		if (dtGameScale != 1)
		{
			dtGame *= dtGameScale;
		}
	}
		dtTimer.Start();
	//frames since start
	frameCount++;

	switch (gameState)
	{
	case GameState_NONE:
		break;
	case GameState_PLAYING:
		gameTimer += dtGame;
		break;
	case GameState_PAUSE:
		dtGame = 0;
		break;
	case GameState_STOP:
		gameTimer = 0.0f;
		dtGame = 0;
		gameState = GameState_NONE;
		break;
	case GameState_TICK:
		if (!oneFrame)
			oneFrame = true;
		else
		{
			gameState = GameState_PAUSE;
			oneFrame = false;
		}
		gameTimer += dtGame;
		break;
	default:
		break;
	}


	//FPS app window
	fpsLog.push_back(1 / dtReal);

	if (fpsLog.size() > 75)
	{
		fpsLog.erase(fpsLog.begin());
	}
	return UPDATE_CONTINUE;
}

update_status ModuleTime::Update()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);

	return UPDATE_CONTINUE;
}

update_status ModuleTime::PostUpdate()
{
	BROFILER_CATEGORY(__FUNCTION__, Profiler::Color::PapayaWhip);


	msLog.push_back(dtReal * 1000);

	if (msLog.size() > 75)
	{
		msLog.erase(msLog.begin());
	}

	return UPDATE_CONTINUE;
}

void ModuleTime::SetScaleGame(float scale)
{
	dtGameScale = scale;
}

uint ModuleTime::GetFrameCount() const
{
	return frameCount;
}
float ModuleTime::Getdt() const
{
	return dtReal;
}
float ModuleTime::GetStartGame() const
{
	return startsTime;
}
float ModuleTime::GetdtGame() const
{
	return dtGame;
}
float ModuleTime::GetGameScale() const
{
	return dtGameScale;
}
float ModuleTime::GetGameTimer() const
{
	return gameTimer;
}

update_status ModuleTime::Save(JSON_Object* object)
{

	return UPDATE_CONTINUE;
}

update_status ModuleTime::Load(JSON_Object * object)
{

	return UPDATE_CONTINUE;
}
