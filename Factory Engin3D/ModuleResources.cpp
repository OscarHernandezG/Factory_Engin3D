#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"


ModuleResources::ModuleResources(Application * app, bool start_enabled): Module(app, start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
	return true;
}

update_status ModuleResources::Update()
{

	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	return true;
}
