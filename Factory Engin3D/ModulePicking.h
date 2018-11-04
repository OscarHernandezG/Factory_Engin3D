#ifndef __ModulePicking_H__
#define __ModulePicking_H__

#include "Module.h"
#include "Globals.h"

class ModulePicking : public Module
{
public:
	ModulePicking(Application* app, bool start_enabled = true);
	~ModulePicking();

	bool Start();
	bool CleanUp();
	update_status Update(float dt);

public:
	RayLine ray;
};

#endif // !__ModulePicking_H__
