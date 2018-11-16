#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"

#include "Resource.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"


class ModuleResources: public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true);
	~ModuleResources();

	bool Start();
	update_status Update();
	bool CleanUp();

	bool ExistFile(const char* path);
	vector<string> ReadFolder(const char * path);

	Resource* FindLoadedResource(const char* path, ResourceType type);

	ResourceTexture* LoadTexture(char * path);


private:
	bool RealLoadTexture(char* path, uint &opengGlTexture);

public:
	std::list<Resource*> resources;
};
#endif // !__ModuleResource__H
