#include "Globals.h"
#include "Application.h"
#include "Module.h"
#include "ModuleImporter.h"

#include <fstream>
#include <iostream>
using namespace std;

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init()
{

	CreateDirectory("Llibrary",NULL);
	CreateDirectory("Llibrary/Meshes", NULL);
	CreateDirectory("Llibrary/Textures", NULL);
	CreateDirectory("Llibrary/Scenes", NULL);

	return true;
}

void ModuleImporter::DistributeFile(const char* file, bool needPath)
{
	string filePath(file);
	string extension = filePath.substr(filePath.find_last_of(".") + 1);

	if (!extension.compare("fbx") || !extension.compare("obj"))
	{
		if (needPath)
			GetFullPath(filePath, LlibraryType_MESH);
		App->geometry->UpdateMesh(filePath.data());
	}
	else if (!extension.compare("png") || !extension.compare("dds") || !extension.compare("jpg") || !extension.compare("jpeg") || !extension.compare("tga"))
	{
		if (needPath)
			GetFullPath(filePath, LlibraryType_TEXTURE);
		App->geometry->UpdateTexture(filePath.data());
	}
}

void ModuleImporter::GetFullPath(string& file, LlibraryType type)
{
	string fullPath = "Assets\\";

	switch (type)
	{
	case LlibratyType_NONE:
		break;
	case LlibraryType_TEXTURE:
		fullPath.append("textures\\");
		break;
	case LlibraryType_MESH:
		fullPath.append("models\\");
		break;
	case LlibraryType_MATERIAL:
		break;
	case LlibraryType_SCENE:
		break;
	default:
		break;
	}

	fullPath.append(file);

	file = fullPath.data();
}


void ModuleImporter::SaveFile(const char* path, uint size, char* outputFile, LlibraryType type, uint uuid)
{
	string direction = GetDirectionName(path, type, uuid);

	ofstream newFile (direction.c_str(), ios::out | ios::binary);
	if (newFile.is_open())
	{
		newFile.write(outputFile, size);
		newFile.close();
	}
}

char* ModuleImporter::LoadFile(const char* path, LlibraryType type, uint number)
{
	string direction = GetDirectionName(path, type, number);
	char *text = nullptr;
	ifstream loadFile(direction.c_str(), ios::out | ios::binary);
	if (loadFile.is_open())
	{
		loadFile.seekg(0, loadFile.end);
		uint size = loadFile.tellg();
		text = new char[size];

		loadFile.seekg(0, loadFile.beg);

		loadFile.read(text,size);
		loadFile.close();
	}

	return text;
}

string ModuleImporter::GetDirectionName(const char* path, LlibraryType type, uint uuid)
{
	string filePath(path);
	string goodFile = "Llibrary/";

	switch (type)
	{
	case LlibratyType_NONE:
		break;
	case LlibraryType_TEXTURE:
		TextureDirection(goodFile, filePath);
		break;
	case LlibraryType_MESH:
		MeshDirection(filePath, goodFile, uuid);
		break;
	case LlibraryType_MATERIAL:
		break;
	case LlibraryType_SCENE:
		SceneDirection(filePath, goodFile);
		break;
	default:
		break;
	}

	return goodFile;
}

void ModuleImporter::TextureDirection(std::string &goodFile, std::string &filePath)
{
	goodFile += "Textures/";

	uint initialPos = filePath.find_last_of("\\") + 1;
	uint finalPos = filePath.find_last_of(".") + 1;

	goodFile += filePath.substr(initialPos, (finalPos - initialPos)) + "dds";
}

void ModuleImporter::MeshDirection(std::string &filePath, std::string &goodFile, uint uuid)
{
	goodFile += "Meshes/";

	goodFile += to_string(uuid);
	goodFile += ".fty";
}

void ModuleImporter::SceneDirection(std::string &filePath, std::string &goodFile)
{
	goodFile += "Scenes/";

	uint initialPos = filePath.find_last_of("\\") + 1;
	uint finalPos = filePath.find_last_of(".");

	goodFile += filePath.substr(initialPos, (finalPos - initialPos));
	goodFile += ".json";
	// TODO .json to .scene
	// Now in .json to improve 
}