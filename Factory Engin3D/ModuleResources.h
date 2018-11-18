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
	void ReadFolder(const char * path, std::vector<AssetsHierarchy>& filesStr);

	Resource* FindLoadedResource(const char* path, ResourceType type);

	ResourceMesh* LoadMesh(uint name);
	ResourceTexture* LoadTexture(char * path);

private:
	bool RealLoadMesh(char * buffer, ResourceMesh*& mesh, const char* name);
	void LoadColorGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	void LoadTextureUVGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	void LoadVertexGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	void LoadIndexGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	bool RealLoadTexture(char* path, uint &opengGlTexture);

public:
	std::list<Resource*> resources;
};
#endif // !__ModuleResource__H
