#include "ModuleRenderer3D.h"
#include "ModuleResources.h"




ModuleResources::ModuleResources(Application * app, bool start_enabled): Module(app, start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
	return false;
}

update_status ModuleResources::Update()
{

	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	return false;
}
