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

	MeshNode LoadMeshBuffer(const aiScene * scene, uint index, char* path);

	void LoadMeshTextureCoords(MeshBuffer &buffer, aiMesh* newMesh);

	void LoadMeshIndex(aiMesh* newMesh, MeshBuffer &buffer);

	void LoadMeshVertex(MeshBuffer &buffer, aiMesh* newMesh);

	MeshNode LoadMeshNode(const aiScene* scene, aiNode* node, char* path);

	float4x4 AiNatrixToFloatMat(const aiMatrix4x4 &aiMat);

	MeshNode LoadMesh(char* path);

	void SaveMeshImporter(MeshBuffer newCurrentBuffer, const char * path, uint uuid);

	std::vector<MeshBuffer*> LoadMeshImporter(const char * path, const std::vector<MeshNode>& nodes);

	std::vector<MeshBuffer*> LoadMeshImporterUUID(const std::vector<uint>& nodes);

	MeshBuffer* LoadBufferGPU(char * buffer, int id = 0);

	vector<MeshBuffer*> LoadMeshImporterUUID(const vector<uint>& nodes);

	MeshBuffer* LoadBufferGPU(char * buffer, int id = 0);

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

	uint LoadTexture(char* path) const;

	void UpdateTexture(char* path);

	Camera* GetPlayingCamera() const;
public:
	GameObject* currentGameObject = nullptr;
	GameObject* bHouse = nullptr;

	std::vector<MeshNode> nodes;
	std::vector<MeshBuffer*> loadedMeshes;

	uint textureID = 0u;
	uint numFaces = 0u;

	char* droppedFileDir = nullptr;

	AABB currentMeshBB = AABB(float3::zero, float3::zero);

	std::string destination;

	GameObject* plane = nullptr;

private:
	Camera * playingCamera = nullptr;
};
#endif // !__ModuleGeometryManager_H__
