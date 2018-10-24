#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_status PreUpdate(float dt);
	void CopyFilee(const char * file, const char * futureFile);
	void DistributeFile(char * file);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	bool GetIsKeyPressed() const
	{
		return keyPressed;
	}
	
	bool GetIsMousePressed() const
	{
		return mousePressed;
	}
private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	bool keyPressed = false;
	bool mousePressed = false;
	//int mouse_z_motion;
};