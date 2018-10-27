#include "Globals.h"
#include "Application.h"
#include "Module.h"
#include "ModuleImporter.h"

#include <fstream>


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

void ModuleImporter::SaveFile(const char* path, uint size, char* outputFile)
{
	string direction = GetDirectionName(path,0);

	ofstream newFile(direction.c_str(), ofstream::out || ofstream::binary);
	newFile.write(outputFile, size);
	newFile.close();
}


string ModuleImporter::GetDirectionName(const char* path, uint type)
{
	string filePath(path);
	string goodFile = "Llibrary";

	uint initialPos = filePath.find_last_of("\\");
	uint finalPos = filePath.find_last_of(".");

	goodFile += filePath.substr(initialPos, (finalPos-initialPos));
	goodFile += ".fty";

	return goodFile;
}