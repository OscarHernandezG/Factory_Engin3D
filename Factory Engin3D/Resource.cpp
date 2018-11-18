#include "Resource.h"



Resource::Resource(ResourceType type, const char* path)
{
	this->type = type;
	this->file = path;
}

Resource::~Resource()
{
	file.clear();
}

bool Resource::operator==(Resource comp)
{
	bool ret = false;

	if (this->type == comp.type)
		if (std::strcmp(this->file.data(), comp.file.data()) == 0)
			ret = true;

	return ret;
}
