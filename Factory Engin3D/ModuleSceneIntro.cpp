#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleImGui.h"

#include "Primitive.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");

	bool ret = true;

	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	//warrior = LoadMesh();
	return ret;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	update_status status = UPDATE_CONTINUE;

	return status;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{	
	return true;
}

Mesh ModuleSceneIntro::LoadMesh(char* path)
{
	Mesh mesh;
	if (path != nullptr)
	{
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
					currentBuffer.vertex.size = currentMesh->mNumVertices;
					currentBuffer.vertex.buffer = new float[currentBuffer.vertex.size * 3];

					memcpy(currentBuffer.vertex.buffer, currentMesh->mVertices, sizeof(float) * currentBuffer.vertex.size);

					LOG("New mesh loaded with %d vertices", currentBuffer.vertex.size);

					if (currentMesh->HasFaces())
					{
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
	}
	return mesh;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	warrior.Render();
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Draw3D(bool fill, bool wire)
{
	PrimitivePlane plane;
	plane.color = { 1, 1, 1, 1 };
	plane.axis = true;
	plane.Render();

	//PrimitiveCube cube;
	//cube.fill = fill;
	//cube.wire = wire;
	//cube.Render();

	warrior.wire = wire;
	warrior.Render();
}