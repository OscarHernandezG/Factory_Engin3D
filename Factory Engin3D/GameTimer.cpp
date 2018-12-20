// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "GameTimer.h"
#include "Application.h"
// ---------------------------------------------
GameTimer::GameTimer()
{
	App->time->gameTimerList.push_back(this);
}

GameTimer::~GameTimer()
{
	App->time->gameTimerList.remove(this);
}

void GameTimer::Update(float dt)
{
	if(running)
		time += dt;
}

// ---------------------------------------------
void GameTimer::Start()
{
	running = true;
	time = 0;
}
// ---------------------------------------------
void GameTimer::Continue()
{
	running = true;
}

// ---------------------------------------------
void GameTimer::Stop()
{
	running = false;
	time = 0;
}

// ---------------------------------------------
void GameTimer::Pause()
{
	running = false;
}
// ---------------------------------------------
Uint32 GameTimer::Read() const
{
	return time * 1000.0f;
}

float GameTimer::ReadSec() const
{
	return time;
}
