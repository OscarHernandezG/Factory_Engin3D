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

Mesh ModuleSceneIntro::LoadMesh()
{
	Mesh mesh;
	char* filePath = "assets/warrior.FBX";
	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

	bool isSceneLoad = false;
	if (scene != nullptr) {
		if (scene->HasMeshes())
		{
			aiMesh* aiMesh = (*scene->mMeshes);
			for (int i = 0; i < scene->mNumMeshes; ++i)
			{
				mesh.numVertex = aiMesh->mNumVertices;
				mesh.vertex = new float[mesh.numVertex * 3];

				memcpy(mesh.vertex, aiMesh->mVertices, sizeof(float) * mesh.numVertex);

				LOG("New mesh loaded with %d vertices", mesh.numVertex);

				if (aiMesh->HasFaces())
				{
					mesh.numIndex = aiMesh->mNumFaces * 3;
					mesh.index = new uint[mesh.numIndex];
					for (uint index = 0; index < aiMesh->mNumFaces; ++index)
					{
						if (aiMesh->mFaces[index].mNumIndices != 3)
							LOG("Achtung, geometry faces != 3 indices")
						else
							memcpy(&mesh.index[i * 3], aiMesh->mFaces[i].mIndices, sizeof(uint) * 3);
					}
				}

				glGenBuffers(1, (GLuint*)&(mesh.idIndex));
				glBindBuffer(GL_ARRAY_BUFFER, mesh.idIndex);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, mesh.index, GL_STATIC_DRAW);

				glGenBuffers(1, (GLuint*)&(mesh.idVertex));
				glBindBuffer(GL_ARRAY_BUFFER, mesh.idVertex);
				glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * 36, mesh.vertex, GL_STATIC_DRAW);
			}

			isSceneLoad = true;
		}
		aiReleaseImport(scene);
	}

	else
		LOG("Error loading scene %s", filePath);

	return mesh;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::Draw3D(bool fill, bool wire)
{
	PrimitivePlane plane;
	plane.axis = true;
	plane.Render();

	PrimitiveCube cube;
	cube.fill = fill;
	cube.wire = wire;
	cube.Render();
}