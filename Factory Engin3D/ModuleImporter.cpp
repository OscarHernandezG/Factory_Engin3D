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

void ModuleImporter::SaveFile(const char* path, uint size, char* outputFile, LlibraryType type)
{
	string direction = GetDirectionName(path, type);

	ofstream newFile (direction.c_str(), ios::out | ios::binary);
	if (newFile.is_open())
	{
		newFile.write(outputFile, size);
		newFile.close();
	}
}

void ModuleImporter::LoadFile()
{

}

string ModuleImporter::GetDirectionName(const char* path, LlibraryType type)
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

		uint initialPos = filePath.find_last_of("\\") + 1;
		uint finalPos = filePath.find_last_of(".");

		goodFile += filePath.substr(initialPos, (finalPos - initialPos));
		goodFile += ".fty";
		break;
	case LlibraryType_MATERIAL:
		break;
	default:
		break;
	}

	return goodFile;
}