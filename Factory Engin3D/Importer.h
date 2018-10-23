#include "Module.h"
#include "Application.h"

struct String;

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool start_enabled = true) : Module(app, start_enabled) {};
	~ModuleImporter() {};

	virtual bool Import(const char* file, const char* path, std::string outputFile) { return true; };
	virtual bool Load() { return true; };

private:

};
