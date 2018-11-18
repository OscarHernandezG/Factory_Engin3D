#ifndef __ModuleGeometry_H__
#define __ModuleGeometry_H__
#include "Module.h"

#include "Transform.h"
#include "Geometries.h"
#include "Camera.h"

#include "Assimp/include/matrix4x4.h"

#include <vector>

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceTexture.h"

struct aiMesh;
struct aiScene;
struct aiNode;
struct MeshBuffer;

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


// Load New Mesh
// Main method to load a new mesh, this method will load a fbx (loading all meshes and textures)
// and create GameObjects for every mesh inside the fbx.
// ====================================================================================
	void UpdateMesh(const char * path);

//--------------------------------------------------------------------------
// Load mesh from path
	MeshNode LoadMesh(const char* path);
// Load a mesh node (recursive)
	MeshNode LoadMeshNode(const aiScene * scene, aiNode * node);
// Load a mesh buffer
	MeshNode LoadMeshBuffer(const aiScene * scene, uint index);

// Mesthods to load a mesh buffer
	void LoadMeshVertex(ResourceMesh& buffer, aiMesh* newMesh);
	void LoadMeshIndex(aiMesh* newMesh, ResourceMesh& buffer);
	void LoadMeshTextureCoords(ResourceMesh& buffer, aiMesh* newMesh);

//--------------------------------------------------------------------------
// Save Mesh in Importer
	void SaveMeshImporter(ResourceMesh newCurrentBuffer, const char * path, uint uuid);

//--------------------------------------------------------------------------
// Load Mesh from Importer
	void LoadMeshImporter(const char * path, const std::vector<MeshNode>& nodes, std::vector<ResourceMesh*>& buffer);
// Load textures from Importer
	void LoadTextureImporter(std::vector<Textures>& texturesToLoad);

//--------------------------------------------------------------------------
// Load the GameObject from a MeshNode (recursive)
	GameObject* LoadGameObjectsFromMeshNode(MeshNode node, GameObject * father);
	GameObject* LoadEmptyGameObjectsFromMeshNode(MeshNode node, GameObject * father);
// ====================================================================================


// Create a float4x4 from an aiMat4x4
	float4x4 AiNatrixToFloatMat(const aiMatrix4x4& aiMat);

// Get the higher or lower value from 2 floats
	inline void Higher(float& val1, float val2);
	inline void Lower(float& val1, float val2);


// Load meshes from UUID
	void LoadMeshImporterUUID(const std::vector<uint>& nodes, std::vector<ResourceMesh*>& buffers);


// Bounding Box
//----------------------------------------------------------------
// Load
	AABB LoadBoundingBox(Buffer<float> vertex);
// Calc
	float3 CalcBBPos(math::AABB* boundingBox) const;
//Get
	float3 GetBBPos() const;
	float3 GetCurrentMeshPivot() const;
//----------------------------------------------------------------

// Get the In-Game camera
	Camera* GetPlayingCamera() const;

public:
	// The GameObject that is currently selectet
	GameObject* currentGameObject = nullptr;

	// The plane of the scene
	GameObject* plane = nullptr;

	//The current camera while playing
	GameObject* cameraObject = nullptr;

	// MeshNodes and Meshes loaded from a fbx
	std::vector<MeshNode> nodes;	// Can this be temp?
	std::vector<ResourceMesh*> currentMeshes;	// Can this be temp?

	// Textures that have to be loaded
	std::vector<Textures> texturesToLoad;	// Can this be temp?

private:
	Camera* playingCamera = nullptr;
};
#endif // !__ModuleGeometryManager_H__
