#ifndef __GAME_TIMER_H__
#define __GAME_TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class GameTimer
{
public:

	// Constructor
	GameTimer();

	void Start();
	void Continue();
	void Stop();

	Uint32 Read();
	float ReadSec();

private:

	bool	running = false;
	bool	paused = false;
	Uint32	started_at = 0u;
	Uint32	stopped_at = 0u;
	Uint32  paused_at = 0u;
	Uint32	restarted_at = 0u;
};

#endif //__Game_TIMER_H__