#ifndef __ModuleResources_H__
#define __ModuleResources_H__

#include "Module.h"

#include "Resource.h"

#include "ResourceTexture.h"
#include "ResourceMesh.h"


struct ParticleUV
{
	std::vector<uint> textureIDs;
	uint rows = 0;
	uint columns = 0;
};


class ModuleResources: public Module
{
public:
	ModuleResources(Application* app, bool start_enabled = true);
	~ModuleResources();

	bool Init();
	update_status Update();
	bool CleanUp();

	bool ExistFile(const char* path);
	void ReadFolder(const char * path, std::vector<AssetsHierarchy>& filesStr);

	void Remove(Resource* resource);

	Resource* FindLoadedResource(const char* path, ResourceType type);

	bool IsResourceLoaded(Resource* res);

	ResourceMesh* LoadMesh(uint name);
	ResourceMesh* LoadMesh(const MeshNode& mesh);

	ResourceTexture* LoadTexture(const char * path);

	void GetResources(std::vector<Resource*> &resource, ResourceType type);

	ParticleUV LoadTextureUV(int rows, int columns);

	bool FindTextureUV(int rows, int columns, ParticleUV& textureUV);

private:
	bool RealLoadMesh(char * buffer, ResourceMesh*& mesh, const char* name);
	void LoadColorGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	void LoadTextureUVGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	void LoadVertexGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);
	void LoadIndexGPU(char * &cursor, uint &bytes, ResourceMesh * bufferImporter);

	bool RealLoadTexture(const char* path, uint &opengGlTexture);

public:
	std::list<Resource*> resources;

private:
	ParticleUV defaultTextureUV;
	std::list<ParticleUV> particleTextureUV;
};
#endif // !__ModuleResource__H
