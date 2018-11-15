#include "Application.h"
#include "ModuleGeometry.h"
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
#include "Texture.h"

#include "pcg-c-basic-0.9/pcg_basic.h"

using namespace std;

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

MeshNode ModuleGeometry::LoadMeshBuffer(const aiScene* scene, uint index)
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
			texturesToLoad.push_back({ texturePath.substr(texturePath.find_last_of("\\") + 1), newMesh->mMaterialIndex });
		
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

	tempBuffer.buffer.boundingBox = LoadBoundingBox(tempBuffer.buffer.vertex);


	return tempBuffer;
}

void ModuleGeometry::LoadMeshTextureCoords(MeshBuffer &buffer, aiMesh * newMesh)
{
	buffer.texture.size = newMesh->mNumVertices * 2;
	buffer.texture.buffer = new float[buffer.texture.size];
	for (int currVertices = 0; currVertices < newMesh->mNumVertices; ++currVertices)
	{
		memcpy(&buffer.texture.buffer[currVertices * 2], &newMesh->mTextureCoords[0][currVertices], sizeof(float) * 2);
	}
}

void ModuleGeometry::LoadMeshIndex(aiMesh * newMesh, MeshBuffer &buffer)
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

void ModuleGeometry::LoadMeshVertex(MeshBuffer &buffer, aiMesh * newMesh)
{
	buffer.vertex.size = newMesh->mNumVertices * 3;
	buffer.vertex.buffer = new float[buffer.vertex.size];

	memcpy(buffer.vertex.buffer, newMesh->mVertices, sizeof(float) * buffer.vertex.size);

	LOG("New mesh loaded with %d vertices", buffer.vertex.size);
}

MeshNode ModuleGeometry::LoadMeshNode(const aiScene* scene, aiNode* node)
{
	MeshNode meshNode;

	if (node->mNumMeshes > 0)
	{
		MeshNode currMeshBuff = LoadMeshBuffer(scene, node->mMeshes[0]);
		meshNode = currMeshBuff;
	}
	for (int child = 0; child < node->mNumChildren; ++child)
	{
		meshNode.childs.push_back(LoadMeshNode(scene, node->mChildren[child]));
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

MeshNode ModuleGeometry::LoadMesh(const char* path)
{
	MeshNode meshRoot;
	if (path != nullptr)
	{
		const char* filePath = path;
		const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);

		if (scene != nullptr)
		{
			if (scene->HasMeshes())
				meshRoot = LoadMeshNode(scene, scene->mRootNode);

			aiReleaseImport(scene);
		}

		else
			LOG("Error loading geometry %s", filePath);

	}
	return meshRoot;
}

void ModuleGeometry::SaveMeshImporter(MeshBuffer newCurrentBuffer, const char* path, uint uuid)
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

void ModuleGeometry::LoadMeshImporter(const char* path, const vector<MeshNode>& nodes, vector<MeshBuffer*>& buffers)
{
	char* buffer = nullptr;
	for (vector<MeshNode>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		int i = (*iterator).id;

		buffer = App->importer->LoadFile(path, LlibraryType_MESH, (*iterator).componentUUID);

		if (buffer != nullptr)
		{
			MeshBuffer* curr = LoadBufferGPU(buffer, i);

			curr->uuid = (*iterator).componentUUID;
			buffers.push_back(curr);
		}
	}
}

void ModuleGeometry::LoadTextureImporter(vector<Textures>& nodes, vector<Textures>& textures)
{
	char* buffer = nullptr;
	for (vector<Textures>::iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		string path("Assets\\textures\\");
		string name((*iterator).path);
		path.append(name);

		uint textureId = LoadTexture((char*)path.data());

		if (textureId > 0)
		{
			(*iterator).id = textureId;
			textures.push_back(*iterator);
		}
	}
}

vector<MeshBuffer*> ModuleGeometry::LoadMeshImporterUUID(const vector<uint>& nodes)
{
	char* buffer = nullptr;
	vector<MeshBuffer*> buffers;

	for (vector<uint>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		buffer = App->importer->LoadFile("", LlibraryType_MESH, *iterator);

		if (buffer != nullptr)
		{
			MeshBuffer* curr = LoadBufferGPU(buffer);
			curr->uuid = *iterator;

			buffers.push_back(curr);
		}
	}
	return buffers;
}

MeshBuffer* ModuleGeometry::LoadBufferGPU(char * buffer, int id)
{
	MeshBuffer* bufferImporter = new MeshBuffer();
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

	vector<MeshBuffer*>::iterator currentMeshBuffer;
	for (currentMeshBuffer = loadedMeshes.begin(); currentMeshBuffer != loadedMeshes.end(); ++currentMeshBuffer)
	{
		if (node.id > 0)
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
		for (vector<Textures>::iterator currentTexture = loadedTextures.begin(); currentTexture != loadedTextures.end(); ++currentTexture)
		{
			if ((*currentTexture).id == node.textureId)
			{
				TextureInfo textureInfo;
				textureInfo.textureId = (*currentTexture).textureId;
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


void ModuleGeometry::UpdateMesh(const char* path)
{
	MeshNode tempMesh = LoadMesh(path);
	
	// This GO is the one we have to save in the scene
	GameObject* tempGO = LoadEmptyGameObjectsFromMeshNode(tempMesh, App->gameObject->rootGameObject);


	// Save scene in json and use the info to load the new gameObject
	// TODO: check the UUID from the gameobject, we have to save the uuid in the scene file (components too?)
	//std::sort(loadedMeshes.begin(), loadedMeshes.end());
	JSON_Value* rootValue = json_value_init_object();
	JSON_Object* rootObject = json_value_get_object(rootValue);

	//SaveGameObjectJson(tempGO, rootObject);

	tempGO->Delete();

	//int sizeBuf = json_serialization_size_pretty(rootValue);
	//char* buf = new char[sizeBuf];
	//json_serialize_to_buffer_pretty(rootValue, buf, sizeBuf);
	//App->importer->SaveFile("tempGO", sizeBuf, buf, LlibraryType::LlibraryType_MESH);
	//delete[] buf;
	//json_value_free(rootValue);

	sort(nodes.begin(), nodes.end());
	nodes.erase(unique(nodes.begin(), nodes.end()), nodes.end());

	for (vector<MeshNode>::const_iterator iterator = nodes.begin(); iterator != nodes.end(); ++iterator)
	{
		SaveMeshImporter((*iterator).buffer, path, (*iterator).componentUUID);
	}

	LoadMeshImporter(path, nodes, loadedMeshes);

	//LoadTextureImporter(texturesToLoad, loadedTextures);

	GameObject* newGameObject = LoadGameObjectsFromMeshNode(tempMesh, App->gameObject->rootGameObject);

	texturesToLoad.clear();

	currentGameObject = newGameObject;
	bHouse = newGameObject;

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

uint ModuleGeometry::LoadTexture(char* path) const
{
	bool isSuccess = true;
	ILuint newTextureID = 0;
	uint opengGlTexture = 0u;
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

			glGenTextures(1, &opengGlTexture);
			glBindTexture(GL_TEXTURE_2D, opengGlTexture);

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
			LOG("Image conversion error, %s", iluErrorString(ilGetError()));
		}
	}
	else
	{
		isSuccess = false;
		LOG("Error loading texture, %s", iluErrorString(ilGetError()));
	}

	ilDeleteImages(1, &newTextureID);

	if (!isSuccess)
		opengGlTexture = 0;

	return opengGlTexture;
}

void ModuleGeometry::UpdateTexture(char* path)
{
	uint tempTexture = LoadTexture(path);
	if (tempTexture != 0)
	{
	//	if (textureID != 0)
	//	{
	//		glDeleteTextures(1, &textureID);
	//	}
	//	textureID = tempTexture;
	}
}

Camera * ModuleGeometry::GetPlayingCamera() const
{
	return playingCamera;
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
	for (vector<MeshBuffer*>::iterator it = App->geometry->loadedMeshes.begin(); it != App->geometry->loadedMeshes.end(); ++it)
	{
		delete *it;
	}
	App->geometry->loadedMeshes.clear();
}

void ModuleGeometry::Draww(GameObject* object)
{
	assert(object);

	for (list<GameObject*>::iterator it = object->childs.begin(); it != object->childs.end(); ++it)
	{
		Draww(*it);
	}

	Mesh* mesh = (Mesh*)(*object).GetComponent(ComponentType_GEOMETRY);
	if (mesh)
	{
		mesh->Render();
	}
}

void ModuleGeometry::LoadDefaultScene()
{
	//App->importer->DistributeFile("assets\\textures\\Baker_house.png");
	App->importer->DistributeFile("assets\\models\\Street.fbx");

	GameObject* cameraObject = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->rootGameObject, "Camera");
	playingCamera = (Camera*)cameraObject->AddComponent(ComponentType_CAMERA, nullptr);
	
	plane = App->gameObject->CreateGameObject(float3::zero, Quat::identity, float3::one, App->gameObject->rootGameObject, "Ground");

	GeometryInfo planeInfo(new PrimitivePlane());
	plane->AddComponent(ComponentType_GEOMETRY, &planeInfo);
}