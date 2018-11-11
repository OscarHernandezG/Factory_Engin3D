#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"

class ModuleResources: public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true);
	~ModuleResources();

	bool Start();
	update_status Update();
	bool CleanUp();


private:

};
#endif // !__ModuleResource__H
