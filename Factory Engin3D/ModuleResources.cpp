#include "Globals.h"
#include "Application.h"
#include "ModuleResources.h"


#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )

#include "DevIL/includex86/IL/il.h"
#include "DevIL/includex86/IL/ilu.h"
#include "DevIL/includex86/IL/ilut.h"

#include <fstream>

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

Resource* ModuleResources::FindLoadedResource(const char* path, ResourceType type)
{
	for (list<Resource*>::const_iterator iterator = resources.begin(); iterator != resources.end(); ++iterator)
	{
		if ((*iterator)->type == type)
			if (strcmp((*iterator)->file.data(), path) == 0)
			{
				return *iterator;
			}
	}
	return nullptr;
}


ResourceTexture* ModuleResources::LoadTexture(char* path)
{
	Resource* resourceLoaded = FindLoadedResource(path, ResourceType::texture);
	if (resourceLoaded)
	{
		resourceLoaded->usage++;
		ResourceTexture* texture = (ResourceTexture*)resourceLoaded;
		return texture;
	}
	else
	{
		ResourceTexture* texture = nullptr;

		uint opengGlTexture = 0u;

		if (!RealLoadTexture(path, opengGlTexture))
			opengGlTexture = 0u;
		else
		{
			texture = new ResourceTexture(path);
			texture->usage = 1;
			texture->SetID(opengGlTexture);

			resources.push_back((Resource*)texture);
		}

	return texture;
	}
}

bool ModuleResources::RealLoadTexture(char* path, uint& opengGlTexture)
{
	bool ret = true;

	ILuint newTextureID = 0;
	ilGenImages(1, &newTextureID);
	ilBindImage(newTextureID);

	if ((bool)ilLoadImage(path))
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);
		if (size > 0) {
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
				App->importer->SaveFile(path, size, (char*)data, LlibraryType_TEXTURE);
		}

		ILinfo info;
		iluGetImageInfo(&info);
		if (info.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		if (ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
		{
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glGenTextures(1, &opengGlTexture);
			glBindTexture(GL_TEXTURE_2D, opengGlTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
				0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
		}
		else
		{
			ret = false;
			LOG("Image conversion error, %s", iluErrorString(ilGetError()));
		}
	}
	else
	{
		ret = false;
		LOG("Error loading texture, %s", iluErrorString(ilGetError()));
	}

	ilDeleteImages(1, &newTextureID);

	return ret;
}
