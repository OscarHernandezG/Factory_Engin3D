#include "Application.h"
#include "ModuleGeometryManager.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "DevIL/includex86/IL/il.h"
#include "DevIL/includex86/IL/ilu.h"
#include "DevIL/includex86/IL/ilut.h"


#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )



ModuleGeometry::ModuleGeometry(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGeometry::~ModuleGeometry()
{}

// Load assets
bool ModuleGeometry::Start()
{
	LOG("Loading Geometry manager");


	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	bool ret = true;


	currentMesh = LoadMesh("assets/BakerHouse.fbx");
	
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
	return true;
}

Mesh* ModuleGeometry::LoadMesh(char* path)
{
	Mesh* mesh = nullptr;
	if (path != nullptr)
	{
		int faces = 0;
		char* filePath = path;
		const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

		bool isSceneLoad = false;
		if (scene != nullptr) {
			if (scene->HasMeshes())
			{
				mesh = new Mesh();

				for (int i = 0; i < scene->mNumMeshes; ++i)
				{
					aiMesh* currentMesh = scene->mMeshes[i];

					MeshBuffer currentBuffer;
					currentBuffer.vertex.size = currentMesh->mNumVertices * 3;
					currentBuffer.vertex.buffer = new float[currentBuffer.vertex.size * 3];

					memcpy(currentBuffer.vertex.buffer, currentMesh->mVertices, sizeof(float) * currentBuffer.vertex.size);

					LOG("New mesh loaded with %d vertices", currentBuffer.vertex.size);

					glGenBuffers(1, (GLuint*)&(currentBuffer.vertex.id));
					glBindBuffer(GL_ARRAY_BUFFER, currentBuffer.vertex.id);
					glBufferData(GL_ARRAY_BUFFER, sizeof(float) * currentBuffer.vertex.size, currentBuffer.vertex.buffer, GL_STATIC_DRAW);

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
						glGenBuffers(1, (GLuint*)&(currentBuffer.index.id));
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currentBuffer.index.id);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * currentBuffer.index.size, currentBuffer.index.buffer, GL_STATIC_DRAW);
					}

					if (currentMesh->HasTextureCoords(0))
					{
						float* textCoords = new float[currentMesh->mNumVertices * 2];
						for (int currVertices = 0; currVertices < currentMesh->mNumVertices; ++currVertices)
						{
							textCoords[currVertices * 2] = currentMesh->mTextureCoords[0][currVertices].x;
							textCoords[currVertices * 2 + 1] = currentMesh->mTextureCoords[0][currVertices].y;
							LOG("x = %.2f, y = %.2f", textCoords[currVertices * 2], textCoords[currVertices * 2 + 1]);
						}
					
						glGenBuffers(1,&currentBuffer.texture.id);
						glBindBuffer(GL_ARRAY_BUFFER, currentBuffer.texture.id);
						glBufferData(GL_ARRAY_BUFFER, currentMesh->mNumVertices * sizeof(float) * 2, textCoords, GL_STATIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, 0);


						delete[] textCoords;
					
					}

					mesh->buffers.push_back(currentBuffer);
				}
				isSceneLoad = true;
			}
			aiReleaseImport(scene);

		LOG("Loaded geometry with %i faces", faces);
		}

		else
			LOG("Error loading geometry %s", filePath);

	}
	return mesh;
}

void ModuleGeometry::UpdateMesh(char* path)
{
	Mesh* tempMesh = LoadMesh(path);
	if (tempMesh != nullptr)
		if (!tempMesh->buffers.empty())
		{
			currentMesh->ClearMesh();
			currentMesh = tempMesh;
		}
}

Primitive* ModuleGeometry::LoadPrimitive(PrimitiveTypes type)
{
	//TODO


	return nullptr;
}

uint ModuleGeometry::LoadTexture(char* path)
{
	uint textureID = 0;

	ilGenImages(1, &textureID);
	ilBindImage(textureID);

	if ((bool)ilLoadImage(path))
	{
		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		if (ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
				0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		}
		else
		{
			LOG("Image conversion error %s", iluErrorString(ilGetError()));
		}
	}
	else
	{
		LOG("Error loading texture %s", iluErrorString(ilGetError()));
	}

	ilDeleteImages(1, &textureID);

	return textureID;
}

void ModuleGeometry::UpdateTexture(char* path)
{
	uint tempTexture = LoadTexture(path);
	if (tempTexture != 0)
		textureID = tempTexture;
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
	//if (cube != nullptr)
	//{
	//	cube->fill = fill;
	//	cube->wire = wire;
	//	cube->Render();
	//}

	PrimitivePlane plane;
	plane.color = { 1, 1, 1, 1 };
	plane.axis = true;
	plane.Render();

	currentMesh->fill = fill;
	currentMesh->wire = wire;
	currentMesh->Render();

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


	//glLineWidth(2.0f);
	//glBindTexture(GL_TEXTURE_2D, textureID);

	//glBegin(GL_TRIANGLES);
	//
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(-0.5f, -0.5f, -0.5f);//a
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(0.5f, -0.5f, -0.5f);//b
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-0.5f, 0.5f, -0.5f);//c
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-0.5f, 0.5f, -0.5f);//c
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(0.5f, -0.5f, -0.5f);//b
	//glTexCoord2f(1.0f, 1.0f);
	//glVertex3f(0.5f, 0.5f, -0.5f);//d

	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(0.5f, 0.5f, -0.5f);//d
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(0.5f, -0.5f, -0.5f);//b
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(0.5f, -0.5f, 0.5f);//f
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(0.5f, -0.5f, 0.5f);//f
	//glTexCoord2f(1.0f, 1.0f);
	//glVertex3f(0.5f, 0.5f, 0.5f);//h
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(0.5f, 0.5f, -0.5f);//d

	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(-0.5f, 0.5f, -0.5f);//c
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(0.5f, 0.5f, -0.5f);//d
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(-0.5f, 0.5f, 0.5f);//g
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(-0.5f, 0.5f, 0.5f);//g
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(0.5f, 0.5f, -0.5f);//d
	//glTexCoord2f(1.0f, 1.0f);
	//glVertex3f(0.5f, 0.5f, 0.5f);//h

	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(-0.5f, 0.5f, 0.5f);//g
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(-0.5f, -0.5f, 0.5f);//e
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-0.5f, -0.5f, -0.5f);//a
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-0.5f, -0.5f, -0.5f);//a
	//glTexCoord2f(1.0f, 1.0f);
	//glVertex3f(-0.5f, 0.5f, -0.5f);//c
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(-0.5f, 0.5f, 0.5f);//g

	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(-0.5f, -0.5f, -0.5f);//a
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(-0.5f, -0.5f, 0.5f);//e
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(0.5f, -0.5f, 0.5f);//f
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(0.5f, -0.5f, 0.5f);//f
	//glTexCoord2f(1.0f, 1.0f);
	//glVertex3f(0.5f, -0.5f, -0.5f);//b
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(-0.5f, -0.5f, -0.5f);//a

	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(0.5f, 0.5f, 0.5f);//h
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(0.5f, -0.5f, 0.5f);//f
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-0.5f, -0.5f, 0.5f);//e
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-0.5f, -0.5f, 0.5f);//e
	//glTexCoord2f(1.0f, 1.0f);
	//glVertex3f(-0.5f, 0.5f, 0.5f);//g
	//glTexCoord2f(1.0f, 0.0f);
	//glVertex3f(0.5f, 0.5f, 0.5f);//h

	//glMatrixMode(GL_PROJECTION);
	//glPopMatrix();

	//glMatrixMode(GL_MODELVIEW);


	//glEnd();
	//glLineWidth(1.0f);

	//glBindTexture(GL_TEXTURE_2D, 0);

}