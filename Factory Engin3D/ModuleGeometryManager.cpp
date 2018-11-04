#include "Application.h"
#include "ModuleGeometryManager.h"
#include "ModuleImporter.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "DevIL/includex86/IL/il.h"
#include "DevIL/includex86/IL/ilu.h"
#include "DevIL/includex86/IL/ilut.h"

#include <fstream>

#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )

#include "GameObject.h"



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


	LoadDefaultScene();
	
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

void ModuleGeometry::DistributeFile(char* file)
{
	string filePath(file);
	string extension = filePath.substr(filePath.find_last_of(".") + 1);


	if (!extension.compare("fbx") || !extension.compare("obj"))
	{
		UpdateMesh(file);
	}
	else if (!extension.compare("png") || !extension.compare("dds") || !extension.compare("jpg") || !extension.compare("jpeg"))
		UpdateTexture(file);
}


MeshInfo ModuleGeometry::LoadMesh(char* path)
{
	MeshInfo meshBuffer;
	if (path != nullptr)
	{
		char* filePath = path;
		const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr)
		{
			// Todo: GameObject name here
			// Todo: Save names in fty
			meshBuffer.name = scene->mRootNode->mName.C_Str();
			if (scene->HasMeshes())
			{
				numFaces = 0u;
				for (int i = 0; i < scene->mNumMeshes; ++i)
				{
					aiMesh* newMesh = scene->mMeshes[i];
					
					MeshBuffer newCurrentBuffer;

					if (i < scene->mRootNode->mNumChildren)
					{
						aiNode* meshName = scene->mRootNode->mChildren[i];

						newCurrentBuffer.name = meshName->mName.C_Str();
					}
					else
						newCurrentBuffer.name = newMesh->mName.C_Str();


					newCurrentBuffer.vertex.size = newMesh->mNumVertices * 3;
					newCurrentBuffer.vertex.buffer = new float[newCurrentBuffer.vertex.size];

					memcpy(newCurrentBuffer.vertex.buffer, newMesh->mVertices, sizeof(float) * newCurrentBuffer.vertex.size);

					LOG("New mesh loaded with %d vertices", newCurrentBuffer.vertex.size);

					if (newMesh->HasFaces())
					{
						numFaces += newMesh->mNumFaces;
						newCurrentBuffer.index.size = newMesh->mNumFaces * 3;
						newCurrentBuffer.index.buffer = new uint[newCurrentBuffer.index.size];

						for (uint index = 0; index < newMesh->mNumFaces; ++index)
						{
							if (newMesh->mFaces[index].mNumIndices != 3)
								LOG("WARNING, geometry faces != 3 indices")
							else
							{
								memcpy(&newCurrentBuffer.index.buffer[index * 3], newMesh->mFaces[index].mIndices, sizeof(uint) * 3);
							}

						}
					}

					if (newMesh->HasTextureCoords(0))
					{
						newCurrentBuffer.texture.size = newMesh->mNumVertices * 2;
						newCurrentBuffer.texture.buffer = new float[newCurrentBuffer.texture.size];
						for (int currVertices = 0; currVertices < newMesh->mNumVertices; ++currVertices)
						{
							memcpy(&newCurrentBuffer.texture.buffer[currVertices * 2], &newMesh->mTextureCoords[0][currVertices], sizeof(float) * 2);
						}

					}

					meshBuffer.meshes.push_back(newCurrentBuffer);

					SaveMeshImporter(newCurrentBuffer,path,i);
				}
				//Todo calculate bounding box
			currentMeshBB = LoadBoundingBox(&meshBuffer);
			LOG("Loaded geometry with %i faces", numFaces);
			}

			aiReleaseImport(scene);
		}

		else
			LOG("Error loading geometry %s", filePath);

	}
	return meshBuffer;
}

void ModuleGeometry::SaveMeshImporter(MeshBuffer newCurrentBuffer, const char* path, int number)
{
	uint ranges[3] = { newCurrentBuffer.index.size, newCurrentBuffer.vertex.size, newCurrentBuffer.texture.size};

	string;

	float size = sizeof(ranges) + sizeof(uint) * newCurrentBuffer.index.size + sizeof(float) * newCurrentBuffer.vertex.size * 3;

	if(newCurrentBuffer.texture.buffer != nullptr)
		size += sizeof(float) * newCurrentBuffer.texture.size * 2;

	char* exporter = new char[size];
	char* cursor = exporter;

	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	cursor += bytes;
	bytes = sizeof(uint) * newCurrentBuffer.index.size;
	memcpy(cursor, newCurrentBuffer.index.buffer, bytes);

	cursor += bytes;
	bytes = sizeof(float)* newCurrentBuffer.vertex.size;
	memcpy(cursor, newCurrentBuffer.vertex.buffer, bytes);

	if (newCurrentBuffer.texture.buffer != nullptr)
	{
		cursor += bytes;
		bytes = sizeof(float)* newCurrentBuffer.texture.size;
		memcpy(cursor, newCurrentBuffer.texture.buffer, bytes);
	}

	App->importer->SaveFile(path,size,exporter, LlibraryType_MESH, number);
	
	delete[] exporter;
}

void ModuleGeometry::LoadMeshImporter(const char* path, list<MeshBuffer>* tempMesh)
{
	int i = 0;
	char* buffer = App->importer->LoadFile(path, LlibraryType_MESH, i);

	// UUID
	while (buffer != nullptr)
	{
		MeshBuffer bufferImporter;
		char* cursor = buffer;

		uint ranges[3];

		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		bufferImporter.index.size = ranges[0];
		bufferImporter.vertex.size = ranges[1];
		bufferImporter.texture.size = ranges[2];

		cursor += bytes;
		bytes = sizeof(uint)* bufferImporter.index.size;
		bufferImporter.index.buffer = new uint[bufferImporter.index.size];
		memcpy(bufferImporter.index.buffer, cursor, bytes);

		glGenBuffers(1, (GLuint*)&(bufferImporter.index.id));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferImporter.index.id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * bufferImporter.index.size, bufferImporter.index.buffer, GL_STATIC_DRAW);

		cursor += bytes;
		bytes = sizeof(float)* bufferImporter.vertex.size;
		bufferImporter.vertex.buffer = new float[bufferImporter.vertex.size];
		memcpy(bufferImporter.vertex.buffer, cursor, bytes);

		glGenBuffers(1, (GLuint*)&(bufferImporter.vertex.id));
		glBindBuffer(GL_ARRAY_BUFFER, bufferImporter.vertex.id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferImporter.vertex.size, bufferImporter.vertex.buffer, GL_STATIC_DRAW);

		cursor += bytes;
		bytes = sizeof(float)* bufferImporter.texture.size;
		bufferImporter.texture.buffer = new float[bufferImporter.texture.size];
		memcpy(bufferImporter.texture.buffer, cursor, bytes);

		glGenBuffers(1, &bufferImporter.texture.id);
		glBindBuffer(GL_ARRAY_BUFFER, bufferImporter.texture.id);
		glBufferData(GL_ARRAY_BUFFER, bufferImporter.texture.size * sizeof(float), bufferImporter.texture.buffer, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		i++;
		buffer = App->importer->LoadFile(path, LlibraryType_MESH, i);
		tempMesh->push_back(bufferImporter);

	}

	delete[] buffer;
}

void ModuleGeometry::UpdateMesh(char* path)
{
	MeshInfo tempMesh = LoadMesh(path);

	if (!tempMesh.meshes.empty())
	{
		//Geometry* mesh = (Geometry*)currentMesh->GetComponent(ComponentType::ComponentType_GEOMETRY);
		//currentMesh->RemoveComponent(mesh);

		list<string> names;
		for (list<MeshBuffer>::iterator iterator = tempMesh.meshes.begin(); iterator != tempMesh.meshes.end(); ++iterator)
		{
			string data((*iterator).name.data());
			names.push_back(data);
		}

		tempMesh.meshes.clear();
		LoadMeshImporter(path, &tempMesh.meshes);

		list<string>::iterator name = names.begin();
		list<MeshBuffer>::iterator mesh = tempMesh.meshes.begin();
		for (; name != names.end() && mesh != tempMesh.meshes.end(); ++name, ++mesh)
		{
			(*mesh).name = (*name);
		}
		GameObject* newGameObject = nullptr;
		if (tempMesh.meshes.size() > 1)
		{
			newGameObject = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, tempMesh.name.data());
			newGameObject->SetABB(tempMesh.boundingBox);

			for (list<MeshBuffer>::iterator iterator = tempMesh.meshes.begin(); iterator != tempMesh.meshes.end(); ++iterator)
			{
				GameObject* temp  = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, newGameObject, (*iterator).name.data());
				temp->SetABB((*iterator).boundingBox);
				Mesh* currMesh = new Mesh(temp);
				currMesh->buffer = *iterator;
				GeometryInfo info(currMesh);
				//info.boundingBox = *currentMeshBB;
				temp->AddComponent(ComponentType_GEOMETRY, &info);
			}
		}
		else
		{
			newGameObject = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, (*tempMesh.meshes.begin()).name.data());
			newGameObject->SetABB(tempMesh.boundingBox);

			Mesh* currMesh = new Mesh(newGameObject);
			currMesh->buffer = *tempMesh.meshes.begin();

			GeometryInfo info(currMesh);
			//info.boundingBox = *currentMeshBB;
			newGameObject->AddComponent(ComponentType_GEOMETRY, &info);
		}
		//GeometryInfo info(tempMesh);
		//info.boundingBox = *currentMeshBB;
		//currentMesh->AddComponent(ComponentType_GEOMETRY, &info);
		currentMesh = newGameObject;
		bHouse = newGameObject;
	}
}

AABB ModuleGeometry::LoadBoundingBox(MeshInfo* mesh)
{
	AABB boundingBox(float3::zero, float3::zero);
	if (mesh != nullptr)
	{
		float3 max, min;
		std::list<MeshBuffer>::iterator iterator = mesh->meshes.begin();
		for (iterator; iterator != mesh->meshes.end(); ++iterator)
		{
			max.x = (*iterator).vertex.buffer[0];
			max.y = (*iterator).vertex.buffer[1];
			max.z = (*iterator).vertex.buffer[2];

			min = max;

			int vertexSize = (*iterator).vertex.size / 3;
			float* buffer = (*iterator).vertex.buffer;
			for (int i = 0; i < vertexSize; ++i)
			{
				Higher(max.x, buffer[i * 3]);
				Higher(max.y, buffer[i * 3 + 1]);
				Higher(max.z, buffer[i * 3 + 2]);

				Lower(min.x, buffer[i * 3]);
				Lower(min.y, buffer[i * 3 + 1]);
				Lower(min.z, buffer[i * 3 + 2]);
			}

			(*iterator).boundingBox = AABB(min, max);
		}

		iterator = mesh->meshes.begin();
		max = (*iterator).boundingBox.maxPoint;
		min = max;
		for (iterator; iterator != mesh->meshes.end(); ++iterator)
		{
				Higher(max.x, (*iterator).boundingBox.maxPoint.x);
				Higher(max.y, (*iterator).boundingBox.maxPoint.y);
				Higher(max.z, (*iterator).boundingBox.maxPoint.z);

				Lower(min.x, (*iterator).boundingBox.minPoint.x);
				Lower(min.y, (*iterator).boundingBox.minPoint.y);
				Lower(min.z, (*iterator).boundingBox.minPoint.z);

		}
		mesh->boundingBox = AABB(min, max);

		// TODO move camera at drop file
		//App->camera->Position = CalcBBPos(boundingBox);
		//App->camera->Look(CalcBBPos(boundingBox), mesh->GetPos(), false);
	}
	return mesh->boundingBox;
}

float3 ModuleGeometry::CalcBBPos(math::AABB* boundingBox) const
{
	float3 distance{ 0,0,0 };
	if (boundingBox != nullptr)
	{
		float3 size = boundingBox->Size();

		float reScale = 1.25;
		distance.x = (size.x / 2) / math::Tan(0.33333333333 * reScale);
		distance.y = (size.y / 2) / math::Tan(0.33333333333 * reScale);
		distance.z = (size.z / 2) / math::Tan(0.33333333333 * reScale);
	}
	return distance;
}

float3 ModuleGeometry::GetBBPos() const
{
	float3 distance{ 0,0,0 };

	float3 size = currentMeshBB.Size();

	float reScale = 1.25;
	distance.x = (size.x / 2) / math::Tan(0.33333333333 * reScale);
	distance.y = (size.y / 2) / math::Tan(0.33333333333 * reScale);
	distance.z = (size.z / 2) / math::Tan(0.33333333333 * reScale);

	return distance + currentMesh->GetPos();
}

float3 ModuleGeometry::GetCurrentMeshPivot() const
{
	if (currentMesh)
		return currentMesh->GetPos();
	else return float3::zero;
}

void ModuleGeometry::Higher(float& val1, float val2)
{
	val1 = val1 > val2 ? val1 : val2;
}

void ModuleGeometry::Lower(float& val1, float val2)
{
	val1 = val1 < val2 ? val1 : val2;
}

uint ModuleGeometry::LoadTexture(char* path) const
{
	bool isSuccess = true;
	ILuint newTextureID = 0;

	ilGenImages(1, &newTextureID);
	ilBindImage(newTextureID);

	if ((bool)ilLoadImage(path))
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);
		if (size > 0) {
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
				App->importer->SaveFile(path, size, (char*)data, LlibraryType_TEXTURE);
		}

		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		if (ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glGenTextures(1, &newTextureID);
			glBindTexture(GL_TEXTURE_2D, newTextureID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
				0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		}
		else
		{
			isSuccess = false;
			LOG("Image conversion error %s", iluErrorString(ilGetError()));
		}
	}
	else
	{
		isSuccess = false;
		LOG("Error loading texture %s", iluErrorString(ilGetError()));
	}

	ilDeleteImages(1, &newTextureID);

	if (!isSuccess)
		newTextureID = 0;

	return newTextureID;
}

void ModuleGeometry::UpdateTexture(char* path)
{
	uint tempTexture = LoadTexture(path);
	if (tempTexture != 0)
	{
		if (textureID != 0)
		{
			glDeleteTextures(1, &textureID);
		}
		textureID = tempTexture;
	}
}


// Update
update_status ModuleGeometry::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleGeometry::PostUpdate(float dt)
{
	//TEMP
	//----------------------------------------------------------------------------------------------
	if (plane != nullptr && plane->GetActive())
	{
		PrimitivePlane* ground = (PrimitivePlane*)plane->GetComponent(ComponentType_GEOMETRY);

		ground->color = { 1, 1, 1, 1 };
		ground->axis = true;
		ground->Render();
	}
	if (bHouse != nullptr)
	{
		if (bHouse->GetActive())
		{
			for (list<GameObject*>::iterator it = bHouse->childs.begin(); it != bHouse->childs.end(); ++it)
			{
				if ((*it)->GetActive())
				{
					Geometry* currentGeometry = (Geometry*)(*it)->GetComponent(ComponentType_GEOMETRY);
					if (currentGeometry)
						if (currentGeometry->GetType() == Primitive_Mesh)
						{
							Mesh* mesh = (Mesh*)currentGeometry;
							mesh->fill = true;
							mesh->wire = false;
							mesh->Render();
						}
				}
			}
			Geometry* currentGeometry = (Geometry*)(bHouse)->GetComponent(ComponentType_GEOMETRY);
			if (currentGeometry)
				if (currentGeometry->GetType() == Primitive_Mesh)
				{
					Mesh* mesh = (Mesh*)currentGeometry;
					mesh->fill = true;
					mesh->wire = false;
					mesh->Render();

				}
		}
	}
	//----------------------------------------------------------------------------------------------
		return UPDATE_CONTINUE;
}
void ModuleGeometry::Draw3D(bool fill, bool wire) const
{
}

void ModuleGeometry::LoadDefaultScene()
{

	currentMesh = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, "BakerHouse");
	App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, currentMesh, "BakerHouse child ");

	App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, "Empty");

	App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, "Root Child ") , "Root child child");

	DistributeFile("assets\\models\\Moon.fbx");
	DistributeFile("assets\\textures\\Baker_house.dds");


	plane = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, "Ground");

	GeometryInfo planeInfo(new PrimitivePlane());
	plane->AddComponent(ComponentType_GEOMETRY, &planeInfo);


	//GameObject* box = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->root, "Box at 0,0,0");

	//GeometryInfo cubeInfo(new PrimitiveCube());
	//box->AddComponent(ComponentType_GEOMETRY, &cubeInfo);

}