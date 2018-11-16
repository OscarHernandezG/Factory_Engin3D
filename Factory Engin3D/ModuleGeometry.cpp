#include "Application.h"
#include "ModuleGeometry.h"
#include "ModuleImporter.h"

#include "glew-2.1.0/include/GL/glew.h"

#include "ResourceMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "DevIL/includex86/IL/il.h"
#include "DevIL/includex86/IL/ilu.h"
#include "DevIL/includex86/IL/ilut.h"

#include <fstream>

#include "GameObject.h"
#include "Texture.h"

#include "pcg-c-basic-0.9/pcg_basic.h"


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

// Load assets
bool ModuleGeometry::CleanUp()
{
	return true;
}

MeshNode ModuleGeometry::LoadMeshBuffer(const aiScene* scene, uint index, char* path)
{
	MeshNode tempBuffer;

	LOG("Mesh index = %u", index);

	aiMesh* newMesh = scene->mMeshes[index];

	LoadMeshVertex(tempBuffer.buffer, newMesh);

	if (newMesh->HasFaces())
	{
		LoadMeshIndex(newMesh, tempBuffer.buffer);
	}

	if (newMesh->HasTextureCoords(0))
	{
		LoadMeshTextureCoords(tempBuffer.buffer, newMesh);

	}

	if (scene->HasMaterials())
	{
		aiMaterial* material = scene->mMaterials[newMesh->mMaterialIndex];
	
		if (material)
		{
			aiString textName;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textName);

			string texturePath(textName.data);
			uint uuid = pcg32_random();

			texturesToLoad[newMesh->mMaterialIndex] = { texturePath.substr(texturePath.find_last_of("\\") + 1), newMesh->mMaterialIndex };
		
			tempBuffer.textureId = newMesh->mMaterialIndex;
			tempBuffer.hasTexture = true;
		};
	}

	if (newMesh->GetNumColorChannels() > 0)
		if (newMesh->HasVertexColors(0))
		{
			tempBuffer.buffer.color.size = tempBuffer.buffer.vertex.size * 4;
			tempBuffer.buffer.color.buffer = new float[tempBuffer.buffer.color.size];
			memcpy(tempBuffer.buffer.color.buffer, newMesh->mColors[0], sizeof(float) * tempBuffer.buffer.color.size);
		}
	tempBuffer.id = index;
	tempBuffer.componentUUID = pcg32_random();

	//SaveMeshImporter(tempBuffer.buffer, path, tempBuffer.componentUUID);

	tempBuffer.buffer.boundingBox = LoadBoundingBox(tempBuffer.buffer.vertex);


	return tempBuffer;
}

void ModuleGeometry::LoadMeshTextureCoords(ResourceMesh& buffer, aiMesh * newMesh)
{
	buffer.texture.size = newMesh->mNumVertices * 2;
	buffer.texture.buffer = new float[buffer.texture.size];
	for (int currVertices = 0; currVertices < newMesh->mNumVertices; ++currVertices)
	{
		memcpy(&buffer.texture.buffer[currVertices * 2], &newMesh->mTextureCoords[0][currVertices], sizeof(float) * 2);
	}
}

void ModuleGeometry::LoadMeshIndex(aiMesh * newMesh, ResourceMesh& buffer)
{
	numFaces += newMesh->mNumFaces;
	buffer.index.size = newMesh->mNumFaces * 3;
	buffer.index.buffer = new uint[buffer.index.size];

	for (uint index = 0; index < newMesh->mNumFaces; ++index)
	{
		if (newMesh->mFaces[index].mNumIndices != 3)
			LOG("WARNING, geometry faces != 3 indices")
		else
		{
			memcpy(&buffer.index.buffer[index * 3], newMesh->mFaces[index].mIndices, sizeof(uint) * 3);
		}

	}
}

void ModuleGeometry::LoadMeshVertex(ResourceMesh& buffer, aiMesh * newMesh)
{
	buffer.vertex.size = newMesh->mNumVertices * 3;
	buffer.vertex.buffer = new float[buffer.vertex.size];

	memcpy(buffer.vertex.buffer, newMesh->mVertices, sizeof(float) * buffer.vertex.size);

	LOG("New mesh loaded with %d vertices", buffer.vertex.size);
}

MeshNode ModuleGeometry::LoadMeshNode(const aiScene* scene, aiNode* node, char* path)
{
	MeshNode meshNode;

	if (node->mNumMeshes > 0)
	{
		LOG("Loading %s Mesh", node->mName.C_Str());
		MeshNode currMeshBuff = LoadMeshBuffer(scene, node->mMeshes[0], path);
		meshNode = currMeshBuff;
	}
	for (int child = 0; child < node->mNumChildren; ++child)
	{
		meshNode.childs.push_back(LoadMeshNode(scene, node->mChildren[child], path));
	}
	
	meshNode.transform = AiNatrixToFloatMat(node->mTransformation);
	meshNode.name = node->mName.C_Str();

	nodes.push_back(meshNode);

	return meshNode;
}

float4x4 ModuleGeometry::AiNatrixToFloatMat(const aiMatrix4x4 & aiMat)
{
	float4x4 mat(aiMat.a1, aiMat.a2, aiMat.a3, aiMat.a4,
				aiMat.b1, aiMat.b2, aiMat.b3, aiMat.b4,
				aiMat.c1, aiMat.c2, aiMat.c3, aiMat.c4,
				aiMat.d1, aiMat.d2, aiMat.d3, aiMat.d4);
	return mat;
}

MeshNode ModuleGeometry::LoadMesh(char* path)
{
	MeshNode meshRoot;
	if (path != nullptr)
	{
		char* filePath = path;
		const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr)
		{
			if (scene->HasMeshes())
			{
				texturesToLoad.clear();
				texturesToLoad.resize(scene->mNumMaterials);
				meshRoot = LoadMeshNode(scene, scene->mRootNode, path);
			}
			aiReleaseImport(scene);
		}

		else
			LOG("Error loading geometry %s", filePath);

	}
	return meshRoot;
}

void ModuleGeometry::SaveMeshImporter(ResourceMesh newCurrentBuffer, const char* path, uint uuid)
{
	uint ranges[4] = { newCurrentBuffer.index.size, newCurrentBuffer.vertex.size, newCurrentBuffer.texture.size, newCurrentBuffer.color.size};

	string;

	float size = sizeof(ranges) + sizeof(uint) * newCurrentBuffer.index.size + sizeof(float) * newCurrentBuffer.vertex.size * 3;

	if(newCurrentBuffer.texture.buffer != nullptr)
		size += sizeof(float) * newCurrentBuffer.texture.size * 2;
	if (newCurrentBuffer.color.buffer != nullptr)
		size += sizeof(float) * newCurrentBuffer.color.size * 4;

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

	if (newCurrentBuffer.color.buffer != nullptr)
	{
		cursor += bytes;
		bytes = sizeof(float)* newCurrentBuffer.color.size;
		memcpy(cursor, newCurrentBuffer.color.buffer, bytes);
	}

	App->importer->SaveFile(path, size, exporter, LlibraryType_MESH, uuid);
	
	delete[] exporter;
}

void ModuleGeometry::LoadMeshImporter(const char* path, const vector<MeshNode>& nodes, vector<ResourceMesh*>& buffers)
{
	char* buffer = nullptr;
	for (vector<MeshNode>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		int i = (*iterator).id;

		buffer = App->importer->LoadFile(path, LlibraryType_MESH, (*iterator).componentUUID);

		if (buffer != nullptr)
		{
			ResourceMesh* curr = LoadBufferGPU(buffer, i);

			curr->uuid = (*iterator).componentUUID;
			buffers.push_back(curr);
		}
	}
}

void ModuleGeometry::LoadTextureImporter(vector<Textures>& texturesToLoad)
{
	char* buffer = nullptr;
	for (vector<Textures>::iterator iterator = texturesToLoad.begin(); iterator != texturesToLoad.end(); ++iterator)
	{
		string path("Assets\\textures\\");
		string name((*iterator).path);
		path.append(name);

		ResourceTexture* textureId = App->resources->LoadTexture((char*)path.data());

		if (!(textureId->GetID() > 0))
			LOG("Warning, texture not loaded");

		(*iterator).texture = textureId;
	}
}

vector<ResourceMesh*> ModuleGeometry::LoadMeshImporterUUID(const vector<uint>& nodes)
{
	char* buffer = nullptr;
	vector<ResourceMesh*> buffers;

	for (vector<uint>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		buffer = App->importer->LoadFile("", LlibraryType_MESH, *iterator);

		if (buffer != nullptr)
		{
			ResourceMesh* curr = LoadBufferGPU(buffer);
			curr->uuid = *iterator;

			buffers.push_back(curr);
		}
	}
	return buffers;
}

ResourceMesh* ModuleGeometry::LoadBufferGPU(char * buffer, int id)
{
	ResourceMesh* bufferImporter = new ResourceMesh(nullptr);
	char* cursor = buffer;

	bufferImporter->id = id;

	uint ranges[4];

	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	bufferImporter->index.size = ranges[0];
	bufferImporter->vertex.size = ranges[1];
	bufferImporter->texture.size = ranges[2];
	bufferImporter->color.size = ranges[3];

	// Index
	cursor += bytes;
	bytes = sizeof(uint)* bufferImporter->index.size;
	bufferImporter->index.buffer = new uint[bufferImporter->index.size];
	memcpy(bufferImporter->index.buffer, cursor, bytes);

	glGenBuffers(1, (GLuint*)&(bufferImporter->index.id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferImporter->index.id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * bufferImporter->index.size, bufferImporter->index.buffer, GL_STATIC_DRAW);

	// Vertex
	cursor += bytes;
	bytes = sizeof(float)* bufferImporter->vertex.size;
	bufferImporter->vertex.buffer = new float[bufferImporter->vertex.size];
	memcpy(bufferImporter->vertex.buffer, cursor, bytes);

	glGenBuffers(1, (GLuint*)&(bufferImporter->vertex.id));
	glBindBuffer(GL_ARRAY_BUFFER, bufferImporter->vertex.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * bufferImporter->vertex.size, bufferImporter->vertex.buffer, GL_STATIC_DRAW);

	//Texture
	cursor += bytes;
	bytes = sizeof(float)* bufferImporter->texture.size;
	bufferImporter->texture.buffer = new float[bufferImporter->texture.size];
	memcpy(bufferImporter->texture.buffer, cursor, bytes);

	glGenBuffers(1, &bufferImporter->texture.id);
	glBindBuffer(GL_ARRAY_BUFFER, bufferImporter->texture.id);
	glBufferData(GL_ARRAY_BUFFER, bufferImporter->texture.size * sizeof(float), bufferImporter->texture.buffer, GL_STATIC_DRAW);

	// Color
	cursor += bytes;
	bytes = sizeof(float)* bufferImporter->color.size;
	bufferImporter->color.buffer = new float[bufferImporter->color.size];
	memcpy(bufferImporter->color.buffer, cursor, bytes);

	glGenBuffers(1, &bufferImporter->color.id);
	glBindBuffer(GL_ARRAY_BUFFER, bufferImporter->color.id);
	glBufferData(GL_ARRAY_BUFFER, bufferImporter->color.size * sizeof(float), bufferImporter->color.buffer, GL_STATIC_DRAW);


	// Clear buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	bufferImporter->boundingBox = LoadBoundingBox(bufferImporter->vertex);

	delete buffer;

	return bufferImporter;
}

GameObject* ModuleGeometry::LoadGameObjectsFromMeshNode(MeshNode node, GameObject* father)
{
	GameObject* newGameObject = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, father, node.name.data());
	newGameObject->SetTransform(node.transform);
	newGameObject->SetABB((node.buffer).boundingBox);

	// TODO remove and use the vector

	vector<ResourceMesh*>::iterator currentMeshBuffer;
	for (currentMeshBuffer = loadedMeshes.begin(); currentMeshBuffer != loadedMeshes.end(); ++currentMeshBuffer)
	{
		if (node.id >= 0)
			if ((*currentMeshBuffer)->id == node.id)
			{
				Mesh* currMesh = new Mesh(newGameObject);

				currMesh->hasColor = node.hasColor;
				currMesh->color = node.color;

				currMesh->buffer = (*currentMeshBuffer);
				currMesh->SetUUID((*currentMeshBuffer)->uuid);
				GeometryInfo info(currMesh);
				newGameObject->AddComponent(ComponentType_GEOMETRY, &info);
				App->sceneIntro->octree.Insert(newGameObject);
				break;
			}
	}


	if (node.hasTexture)
		for (vector<Textures>::iterator currentTexture = texturesToLoad.begin(); currentTexture != texturesToLoad.end(); ++currentTexture)
		{
			if ((*currentTexture).id == node.textureId)
			{
				TextureInfo textureInfo;
				textureInfo.texture = (*currentTexture).texture;
				newGameObject->AddComponent(ComponentType_TEXTURE, &textureInfo);
			}
		}

	for (list<MeshNode>::iterator childs = node.childs.begin(); childs != node.childs.end(); ++childs)
	{
		LoadGameObjectsFromMeshNode(*childs, newGameObject);
	}
	return newGameObject;
}

// Create a new gameObject wich only stores the name, the component geometry asociated and the hierarchy
// This is used to create a temporal gameObject when we load a new geometry to the engine
// This way we create a shell that will serve as a guide to save the geometry in .fty and load the real GameObject afterwards
GameObject* ModuleGeometry::LoadEmptyGameObjectsFromMeshNode(MeshNode node, GameObject* father)
{
	GameObject* newGameObject = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, father, node.name.data());
	newGameObject->SetTransform(node.transform);
	newGameObject->SetABB((node.buffer).boundingBox);


	vector<MeshNode>::iterator currentMeshBuffer;
	for (currentMeshBuffer = nodes.begin(); currentMeshBuffer != nodes.end(); ++currentMeshBuffer)
	{
		if ((*currentMeshBuffer).id == node.id)
		{
			Mesh* currMesh = new Mesh(newGameObject);

			currMesh->SetUUID(node.componentUUID);

			GeometryInfo info(currMesh);
			newGameObject->AddComponent(ComponentType_GEOMETRY, &info);

			//App->sceneIntro->octree.Insert(newGameObject);
			break;
		}
	}

	for (list<MeshNode>::iterator childs = node.childs.begin(); childs != node.childs.end(); ++childs)
	{
		LoadEmptyGameObjectsFromMeshNode(*childs, newGameObject);
	}
	return newGameObject;
}


void ModuleGeometry::UpdateMesh(char* path)
{
	MeshNode tempMesh = LoadMesh(path);
	
	GameObject* tempGO = LoadEmptyGameObjectsFromMeshNode(tempMesh, App->gameObject->rootGameObject);


	JSON_Value* rootValue = json_value_init_object();
	JSON_Object* rootObject = json_value_get_object(rootValue);


	tempGO->Delete();

	sort(nodes.begin(), nodes.end());
	nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

	for (vector<MeshNode>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		if ((*iterator).id >= 0)
		SaveMeshImporter((*iterator).buffer, path, (*iterator).componentUUID);
	}

	LoadMeshImporter(path, nodes, loadedMeshes);
	LoadTextureImporter(texturesToLoad);

	GameObject* newGameObject = LoadGameObjectsFromMeshNode(tempMesh, App->gameObject->rootGameObject);

	texturesToLoad.clear();

	currentGameObject = newGameObject;
}

void ModuleGeometry::SaveGameObjectJson(GameObject* object, JSON_Object* parent)
{
	JSON_Value* newValue = json_value_init_object();
	JSON_Object* objGO = json_value_get_object(newValue);

	json_object_set_value(parent, object->name.data() , newValue);

	json_object_set_number(objGO, "UUID", object->GetUID());

	// Position
	//------------------------------------------------------------------------
	JSON_Value* position = json_value_init_object();
	JSON_Object* positionObj = json_value_get_object(position);

	json_object_set_value(objGO, "Position", position);

	float3 pos = object->GetPos();

	json_object_set_number(positionObj, "X", pos.x);
	json_object_set_number(positionObj, "Y", pos.y);
	json_object_set_number(positionObj, "Z", pos.z);
	
	// Scale
	//------------------------------------------------------------------------
	JSON_Value* scale = json_value_init_object();
	JSON_Object* scalenObj = json_value_get_object(scale);

	json_object_set_value(objGO, "Scale", scale);

	float3 size = object->GetScale();

	json_object_set_number(scalenObj, "X", size.x);
	json_object_set_number(scalenObj, "Y", size.y);
	json_object_set_number(scalenObj, "Z", size.z);

	// Rotation
	//------------------------------------------------------------------------
	JSON_Value* rotation = json_value_init_object();
	JSON_Object* rotationObj = json_value_get_object(rotation);

	json_object_set_value(objGO, "Rotation", rotation);

	Quat rot = object->GetRotation();

	json_object_set_number(rotationObj, "X", rot.x);
	json_object_set_number(rotationObj, "Y", rot.y);
	json_object_set_number(rotationObj, "Z", rot.z);
	json_object_set_number(rotationObj, "W", rot.w);
	//------------------------------------------------------------------------

	json_object_set_number(objGO, "isActive", object->GetActive());

	for (list<GameObject*>::iterator iterator = object->childs.begin(); iterator != object->childs.end(); ++iterator)
	{
		SaveGameObjectJson(*iterator, objGO);
	}
}

AABB ModuleGeometry::LoadBoundingBox(Buffer<float> vertex)
{
	AABB boundingBox(float3::zero, float3::zero);
	float3 max, min;

	if (vertex.size >= 3)
	{
		max.x = vertex.buffer[0];
		max.y = vertex.buffer[1];
		max.z = vertex.buffer[2];

		min = max;

		int vertexSize = vertex.size / 3;
		float* buffer = vertex.buffer;
		for (int i = 0; i < vertexSize; ++i)
		{
			Higher(max.x, buffer[i * 3]);
			Higher(max.y, buffer[i * 3 + 1]);
			Higher(max.z, buffer[i * 3 + 2]);

			Lower(min.x, buffer[i * 3]);
			Lower(min.y, buffer[i * 3 + 1]);
			Lower(min.z, buffer[i * 3 + 2]);
		}

		boundingBox = AABB(min, max);
		// TODO move camera at drop file
		//App->camera->Position = CalcBBPos(boundingBox);
		//App->camera->Look(CalcBBPos(boundingBox), mesh->GetPos(), false);
	}
	return boundingBox;
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

	return distance + currentGameObject->GetPos();
}

float3 ModuleGeometry::GetCurrentMeshPivot() const
{
	if (currentGameObject)
		return currentGameObject->GetPos();
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



void ModuleGeometry::UpdateTexture(char* path)
{
	//uint tempTexture = App->resources->LoadTexture(path);
	//if (tempTexture != 0)
	{
	//	if (textureID != 0)
	//	{
	//		glDeleteTextures(1, &textureID);
	//	}
	//	textureID = tempTexture;
	}
}

update_status ModuleGeometry::PostUpdate()
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
		return UPDATE_CONTINUE;
}

void ModuleGeometry::ClearLoadedMeshes()
{
	for (vector<ResourceMesh*>::iterator it = App->geometry->loadedMeshes.begin(); it != App->geometry->loadedMeshes.end(); ++it)
	{
		delete *it;
	}
	App->geometry->loadedMeshes.clear();
}

void ModuleGeometry::LoadDefaultScene()
{
	//App->importer->DistributeFile("assets\\textures\\Baker_house.png");
	App->importer->DistributeFile("assets\\models\\Street.fbx");

	plane = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->rootGameObject, "Ground");

	GeometryInfo planeInfo(new PrimitivePlane());
	plane->AddComponent(ComponentType_GEOMETRY, &planeInfo);
}