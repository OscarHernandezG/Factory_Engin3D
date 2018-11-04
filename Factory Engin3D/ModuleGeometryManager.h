#ifndef __ModuleGeometryManager_H__
#define __ModuleGeometryManager_H__
#include "Module.h"

#include "Transform.h"
#include "Geometries.h"

class ModuleGeometry : public Module
{
public:
	ModuleGeometry(Application* app, bool start_enabled = true);
	~ModuleGeometry();

	bool Start();
	void LoadDefaultScene();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	void Draw3D(bool fill = true, bool wire = false) const;
	bool CleanUp();

	void DistributeFile(char * file);

	MeshInfo LoadMesh(char* path);

	void SaveMeshImporter(MeshBuffer newCurrentBuffer, const char * path, int number);

	void LoadMeshImporter(const char * path, list<MeshBuffer>* tempMesh);

	void UpdateMesh(char * path);

	AABB LoadBoundingBox(MeshInfo * mesh);

	float3 CalcBBPos(math::AABB* boundingBox) const;

	float3 GetBBPos() const;

	float3 GetCurrentMeshPivot() const;

	void Higher(float& val1, float val2);

	void Lower(float& val1, float val2);

	//Geometry* LoadPrimitive(PrimitiveTypes type);

	uint LoadTexture(char* path) const;

	void UpdateTexture(char* path);

public:
	GameObject* currentMesh = nullptr;
	GameObject* bHouse = nullptr;


	uint textureID = 0;
	uint numFaces = 0;

	char* droppedFileDir = nullptr;

	AABB currentMeshBB = AABB(float3::zero, float3::zero);

	string destination;

	GameObject* plane = nullptr;
};
#endif // !__ModuleGeometryManager_H__
