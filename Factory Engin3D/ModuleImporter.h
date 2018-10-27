#ifndef __ModuleImporter_H__
#define __ModuleImporter_H__

#include "Module.h"
#include "Globals.h"

struct String;

using namespace std;
class ModuleImporter : public Module
{
public:

	ModuleImporter(Application* app, bool start_enabled = true);
	~ModuleImporter();

	bool Init();

	void SaveFile(const char * path, uint size, char* outputFile);

	string GetDirectionName(const char* path, uint type);
	//bool Import(const char* file, const char* path, string outputFile) { return true; };
	//bool Load() { return true; };

private:

};
#endif