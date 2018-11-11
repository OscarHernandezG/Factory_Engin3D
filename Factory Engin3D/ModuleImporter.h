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
	LlibraryType_MATERIAL,
	LlibraryType_SCENE
};
using namespace std;
class ModuleImporter : public Module
{
public:

	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init();

	void SaveFile(const char * path, uint size, char* outputFile, LlibraryType type, uint uuid = 0);

	char* LoadFile(const char * path, LlibraryType type, uint number = 0);

	string GetDirectionName(const char* path, LlibraryType type, uint number = 0);
	void TextureDirection(std::string &goodFile, std::string &filePath);
	void MeshDirection(std::string &filePath, std::string &goodFile, uint number = 0);
	void SceneDirection(std::string & filePath, std::string & goodFile);
};
#endif // !__ModuleImporter_H__