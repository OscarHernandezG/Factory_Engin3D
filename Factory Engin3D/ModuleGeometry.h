#ifndef __ModuleGeometry_H__
#define __ModuleGeometry_H__
#include "Module.h"

#include "Transform.h"
#include "Geometries.h"
#include "Camera.h"

#include "Assimp/include/matrix4x4.h"

#include <vector>

struct aiMesh;
struct aiScene;
struct aiNode;
struct MeshBuffer;

struct Textures
{
	std::string path;
	uint id = 0;

	uint textureId = 0;
};

class ModuleGeometry : public Module
{
public:
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Start();
	void LoadDefaultScene();
	update_status PostUpdate();

	void ClearLoadedMeshes();

	void Draww(GameObject * object);

	bool CleanUp();

	MeshNode LoadMeshBuffer(const aiScene * scene, uint index);

	void LoadMeshTextureCoords(MeshBuffer &buffer, aiMesh* newMesh);

	void LoadMeshIndex(aiMesh* newMesh, MeshBuffer &buffer);

	void LoadMeshVertex(MeshBuffer &buffer, aiMesh* newMesh);

	MeshNode LoadMeshNode(const aiScene * scene, aiNode * node);

	float4x4 AiNatrixToFloatMat(const aiMatrix4x4 &aiMat);

	MeshNode LoadMesh(const char* path);

	void SaveMeshImporter(MeshBuffer newCurrentBuffer, const char * path, uint uuid);

	void LoadMeshImporter(const char * path, const std::vector<MeshNode>& nodes, std::vector<MeshBuffer*>& buffer);

	void LoadTextureImporter(std::vector<Textures>& nodes, std::vector<Textures>& textures);

	std::vector<MeshBuffer*> LoadMeshImporterUUID(const std::vector<uint>& nodes);

	MeshBuffer* LoadBufferGPU(char * buffer, int id = 0);

	GameObject* LoadGameObjectsFromMeshNode(MeshNode node, GameObject * father);

	GameObject * LoadEmptyGameObjectsFromMeshNode(MeshNode node, GameObject * father);

	void UpdateMesh(const char * path);

	void SaveGameObjectJson(GameObject* object, JSON_Object* parent);

	AABB LoadBoundingBox(Buffer<float> vertex);

	float3 CalcBBPos(math::AABB* boundingBox) const;

	float3 GetBBPos() const;

	float3 GetCurrentMeshPivot() const;

	inline void Higher(float& val1, float val2);

	inline void Lower(float& val1, float val2);

	//Geometry* LoadPrimitive(PrimitiveTypes type);

	uint LoadTexture(const char* path) const;

	void UpdateTexture(const char* path);

	Camera* GetPlayingCamera() const;
public:
	GameObject* currentGameObject = nullptr;
	GameObject* bHouse = nullptr;

	std::vector<MeshNode> nodes;
	std::vector<MeshBuffer*> loadedMeshes;

	std::vector<Textures> texturesToLoad;
	std::vector<Textures> loadedTextures;

	uint numFaces = 0u;

	char* droppedFileDir = nullptr;

	AABB currentMeshBB = AABB(float3::zero, float3::zero);

	std::string destination;

	GameObject* plane = nullptr;
	GameObject* cameraObject = nullptr;
private:
	Camera * playingCamera = nullptr;
};
#endif // !__ModuleGeometryManager_H__
