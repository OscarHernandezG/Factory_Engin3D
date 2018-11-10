#ifndef __ModuleTimeManager_H__
#define __ModuleTimeManager_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

enum GameState
{
	GameState_NONE,
	GameState_PLAYING,
	GameState_PAUSE,
	GameState_STOP,
	GameState_TICK
};
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

	void SetScaleGame(float scale);

	uint GetFrameCount() const;
	float Getdt() const;
	float GetStartGame() const;

	float GetdtGame() const;
	float GetGameScale() const;
	float GetGameTimer() const;

	update_status Save(JSON_Object * object);
	update_status Load(JSON_Object * object);

public:
	GameState gameState = GameState_NONE;

	vector<float> fpsLog;
	vector<float> msLog;

	bool oneFrame = false;
private:
	Timer	dtTimer;

	uint frameCount = 0u;			//app graphics frames since game start

	float dtReal = 0.0f;			//last frame time expressed in seconds(Real)
	float startsTime = 0.0f;		//seconds since game start(Real)
	
	float dtGame = 0.0f;			//last frame time expressed in seconds(Game)
	float dtGameScale = 1.0f;		//scale at which time is passing(Game)
	float gameTimer = 0.0f;			//seconds since game start(Game)
};

#endif // __ModuleTimeManager_H__