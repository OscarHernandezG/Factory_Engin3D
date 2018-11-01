#ifndef __ModuleImporter_H__
#define __ModuleImporter_H__

#include "Module.h"
#include "Globals.h"

struct String;

enum LlibraryType
{
	LlibratyType_NONE,
	LlibraryType_TEXTURE,
	LlibraryType_MESH,
	LlibraryType_MATERIAL
};
using namespace std;
class ModuleImporter : public Module
{
public:

	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init();

	void SaveFile(const char * path, uint size, char* outputFile, LlibraryType type, int number);

	char* LoadFile(const char * path, LlibraryType type, int number);

	string GetDirectionName(const char* path, LlibraryType type, int number);
	void MeshDirection(std::string &filePath, std::string &goodFile, int number);
};
#endif