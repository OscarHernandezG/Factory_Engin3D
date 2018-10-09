#include "Application.h"
#include "ModuleGeometryManager.h"
#include "ModuleImGui.h"

#include "Primitive.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


ModuleGeometry::ModuleGeometry(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGeometry::~ModuleGeometry()
{}

// Load assets
bool ModuleGeometry::Start()
{
	LOG("Loading Geometry manager");

	bool ret = true;

	//warrior = LoadMesh();

	frust = new PrimitiveFrustum(0.5f, 1.5f, { 2,0,0 });
	cube = new PrimitiveCube({ 0,2,0 }, 1, 1, 1);

	return ret;
}

update_status ModuleGeometry::PreUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;

	return status;
}

// Load assets
bool ModuleGeometry::CleanUp()
{
	delete frust;
	delete cube;

	return true;
}

Mesh ModuleGeometry::LoadMesh(char* path)
{
	Mesh mesh;
	if (path != nullptr)
	{
		int faces = 0;
		char* filePath = path;
		const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

		bool isSceneLoad = false;
		if (scene != nullptr) {
			if (scene->HasMeshes())
			{
				aiMesh* currentMesh = (*scene->mMeshes);
				for (int i = 0; i < scene->mNumMeshes; ++i)
				{
					currentMesh = scene->mMeshes[i];

					MeshBuffer currentBuffer;
					currentBuffer.vertex.size = currentMesh->mNumVertices * 3;
					currentBuffer.vertex.buffer = new float[currentBuffer.vertex.size * 3];

					memcpy(currentBuffer.vertex.buffer, currentMesh->mVertices, sizeof(float) * currentBuffer.vertex.size);

					LOG("New mesh loaded with %d vertices", currentBuffer.vertex.size);

					if (currentMesh->HasFaces())
					{
						faces += currentMesh->mNumFaces;
						currentBuffer.index.size = currentMesh->mNumFaces * 3;
						currentBuffer.index.buffer = new uint[currentBuffer.index.size];
						for (uint index = 0; index < currentMesh->mNumFaces; ++index)
						{
							if (currentMesh->mFaces[index].mNumIndices != 3)
								LOG("WARNING, geometry faces != 3 indices")
							else
							{
								memcpy(&currentBuffer.index.buffer[index * 3], currentMesh->mFaces[index].mIndices, sizeof(uint) * 3);
							}
						}
					}

					glGenBuffers(1, (GLuint*)&(currentBuffer.index.id));
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentBuffer.index.id);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * currentBuffer.index.size, currentBuffer.index.buffer, GL_STATIC_DRAW);

					glGenBuffers(1, (GLuint*)&(currentBuffer.vertex.id));
					glBindBuffer(GL_ARRAY_BUFFER, currentBuffer.vertex.id);
					glBufferData(GL_ARRAY_BUFFER, sizeof(float) * currentBuffer.vertex.size, currentBuffer.vertex.buffer, GL_STATIC_DRAW);

					mesh.buffers.push_back(currentBuffer);
				}
				isSceneLoad = true;
			}
			aiReleaseImport(scene);

		}

		else
			LOG("Error loading scene %s", filePath);

		LOG("Loaded geometry with %i faces", faces);
	}
	return mesh;
}

Primitive ModuleGeometry::LoadPrimitive(PrimitiveTypes type)
{
	//TODO
}

// Update
update_status ModuleGeometry::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleGeometry::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleGeometry::Draw3D(bool fill, bool wire)
{
	PrimitivePlane plane;
	plane.color = { 1, 1, 1, 1 };
	plane.axis = true;
	plane.Render();

	//if (cube != nullptr)
	//{
	//	cube->fill = fill;
	//	cube->wire = wire;
	//	cube->Render();
	//}

	warrior.fill = fill;
	warrior.wire = wire;
	warrior.Render();

	//SpherePrim sphere;
	//sphere.fill = fill;
	//sphere.wire = wire;
	//sphere.Render();

	//if (frust != nullptr)
	//{
	//	frust->fill = fill;
	//	frust->wire = wire;
	//	frust->Render();
	//}
}