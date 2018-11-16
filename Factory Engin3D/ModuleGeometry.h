#ifndef __ModuleGeometryManager_H__
#define __ModuleGeometryManager_H__
#include "Module.h"

#include "Transform.h"
#include "Geometries.h"

#include "Assimp/include/matrix4x4.h"

#include <vector>

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

struct aiMesh;
struct aiScene;
struct aiNode;

struct Textures
{
	std::string path;
	uint id = 0;

	ResourceTexture* textureResource = nullptr;
};

class ModuleGeometry : public Module
{
public:
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Start();
	void LoadDefaultScene();
	update_status PostUpdate();

	bool CleanUp();

	MeshNode LoadMeshBuffer(const aiScene * scene, uint index, char* path);

	void LoadMeshTextureCoords(ResourceMesh& buffer, aiMesh* newMesh);

	void LoadMeshIndex(aiMesh* newMesh, ResourceMesh& buffer);

	void LoadMeshVertex(ResourceMesh& buffer, aiMesh* newMesh);

	MeshNode LoadMeshNode(const aiScene* scene, aiNode* node, char* path);

	float4x4 AiNatrixToFloatMat(const aiMatrix4x4 &aiMat);

	MeshNode LoadMesh(char* path);

	void SaveMeshImporter(ResourceMesh newCurrentBuffer, const char * path, uint uuid);

	void LoadMeshImporter(const char * path, const vector<MeshNode>& nodes, vector<ResourceMesh*>& buffer);

	void LoadTextureImporter(vector<Textures>& texturesToLoad);

	vector<ResourceMesh*> LoadMeshImporterUUID(const vector<uint>& nodes);

	//ResourceMesh* LoadBufferGPU(char * buffer, int id = 0);

	GameObject* LoadGameObjectsFromMeshNode(MeshNode node, GameObject * father);

	GameObject * LoadEmptyGameObjectsFromMeshNode(MeshNode node, GameObject * father);

	void UpdateMesh(char * path);

	void SaveGameObjectJson(GameObject* object, JSON_Object* parent);

	AABB LoadBoundingBox(Buffer<float> vertex);

	float3 CalcBBPos(math::AABB* boundingBox) const;

	float3 GetBBPos() const;

	float3 GetCurrentMeshPivot() const;

	inline void Higher(float& val1, float val2);

	inline void Lower(float& val1, float val2);

	//Geometry* LoadPrimitive(PrimitiveTypes type);

	void UpdateTexture(char* path);

public:
	GameObject* currentGameObject = nullptr;

	std::vector<MeshNode> nodes;
	std::vector<ResourceMesh*> currentMeshes;

	std::vector<Textures> texturesToLoad;

	uint numFaces = 0u;

	char* droppedFileDir = nullptr;

	AABB currentMeshBB = AABB(float3::zero, float3::zero);

	string destination;

	GameObject* plane = nullptr;
};
#endif // !__ModuleGeometryManager_H__
