#include "Globals.h"
#include "Application.h"
#include "Module.h"
#include "ModuleImporter.h"

#include <fstream>
#include <iostream>

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init()
{

	CreateDirectory("Llibrary",NULL);

	return true;
}

void ModuleImporter::SaveFile(const char* path, uint size, char* outputFile, LlibraryType type, int number)
{
	string direction = GetDirectionName(path, type, number);

	ofstream newFile (direction.c_str(), ios::out | ios::binary);
	if (newFile.is_open())
	{
		newFile.write(outputFile, size);
		newFile.close();
	}
}

char* ModuleImporter::LoadFile(const char* path, LlibraryType type)
{
	string direction = GetDirectionName(path, type,0);
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

string ModuleImporter::GetDirectionName(const char* path, LlibraryType type, int number)
{
	string filePath(path);
	string goodFile = "Llibrary/";

	switch (type)
	{
	case LlibratyType_NONE:
		break;
	case LlibraryType_TEXTURE:
		break;
	case LlibraryType_MESH:
		MeshDirection(filePath, goodFile, number);

		break;
	case LlibraryType_MATERIAL:
		break;
	default:
		break;
	}

	return goodFile;
}

void ModuleImporter::MeshDirection(std::string &filePath, std::string &goodFile, int number)
{
	uint initialPos = filePath.find_last_of("\\") + 1;
	uint finalPos = filePath.find_last_of(".");

	goodFile += filePath.substr(initialPos, (finalPos - initialPos));
	goodFile += to_string(number);
	goodFile += ".fty";
}
