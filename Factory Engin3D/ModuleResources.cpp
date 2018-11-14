#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"

#include <fstream>

using namespace std;
ModuleResources::ModuleResources(Application * app, bool start_enabled): Module(app, start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Start()
{
	return true;
}

update_status ModuleResources::Update()
{

	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	return true;
}

bool ModuleResources::ExistFile(const char * path)
{
	bool ret = false;

	ifstream loadFile(path);
	if (loadFile.is_open())
		ret = true;

	return ret;
}

void ModuleResources::ReadFolder(const char * path, std::vector<AssetsHierarchy>& filesStr)
{
	filesStr.clear();
	string currPath(path);
	
	if (GetFileAttributes(currPath.data()) != INVALID_FILE_ATTRIBUTES)
	{
		currPath.append("*");
		WIN32_FIND_DATA data;
		HANDLE file;
		if ((file = FindFirstFile(currPath.data(), &data)) != INVALID_HANDLE_VALUE)
		{
			do {
				if (strcmp(data.cFileName, "..") && strcmp(data.cFileName, "."))
				{
					int vector = filesStr.size();

					AssetsHierarchy internal;
					internal.file = (data.cFileName);								//Set Name
					filesStr.push_back(internal);									//Push empty childs

					string newPath(path);
					newPath.append(data.cFileName);
					newPath += "\\";

					ReadFolder(newPath.data(), filesStr[vector].childFiles);		//Get childs
				}
			} while (FindNextFile(file, &data) != 0);
		}
		FindClose(file);
	}
}