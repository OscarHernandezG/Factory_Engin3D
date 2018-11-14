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

vector<string> ModuleResources::ReadFolder(const char * path)
{
	string currPath(path);
	vector<string> filesStr;

	if (GetFileAttributes(currPath.data()) != INVALID_FILE_ATTRIBUTES)
	{
		currPath.append("*");
		WIN32_FIND_DATA data;
		HANDLE file;
		if ((file = FindFirstFile(currPath.data(), &data)) != INVALID_HANDLE_VALUE)
		{
			do {
				if (strcmp(data.cFileName, "..") && strcmp(data.cFileName, "."))
					filesStr.push_back(data.cFileName);
			} while (FindNextFile(file, &data) != 0);
		}
		FindClose(file);



		/*for (vector<string>::iterator it = filesStr.begin(); it != filesStr.end(); ++it)
		{
			string newPath(path);
			newPath.append(*it);
			newPath += "\\";
			ReadFolder(newPath.data());
		}*/
	}

	return filesStr;
	/*else if (ExistFile(path))
	{
		bool hi = true;
	}*/
}