// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "GameTimer.h"
#include "Application.h"
// ---------------------------------------------
GameTimer::GameTimer()
{
	if (App)
		App->time->gameTimerList.push_back(this);
}

GameTimer::~GameTimer()
{}

void GameTimer::Update(float dt)
{
	if(running)
		time += dt;
}

// ---------------------------------------------
void GameTimer::Start()
{
	if ((std::find(App->time->gameTimerList.begin(), App->time->gameTimerList.end(), this) == App->time->gameTimerList.end()))
		App->time->gameTimerList.push_back(this);

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
