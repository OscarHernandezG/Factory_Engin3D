// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "GameTimer.h"

// ---------------------------------------------
GameTimer::GameTimer()
{

}

// ---------------------------------------------
void GameTimer::Start()
{
	running = true;
	paused = false;
	started_at = SDL_GetTicks();
}
// ---------------------------------------------
void GameTimer::Continue()
{
	running = true;
	restarted_at = SDL_GetTicks();
}

// ---------------------------------------------
void GameTimer::Stop()
{
	running = false;
	stopped_at = SDL_GetTicks();
	if (paused)
		stopped_at += (restarted_at - stopped_at);
	paused = true;
}

// ---------------------------------------------
Uint32 GameTimer::Read()
{
	if (running && paused)
	{
		return SDL_GetTicks() - started_at - (restarted_at - stopped_at);
	}
	else if (running)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}

float GameTimer::ReadSec()
{
	if (running && paused)
	{
		return (SDL_GetTicks() - started_at - (restarted_at - stopped_at)) / 1000.0f;
	}
	else if (running)
	{
		return (SDL_GetTicks() - started_at) / 1000.0f;
	}
	else
	{
		return (stopped_at - started_at) / 1000.0f;
	}
}
