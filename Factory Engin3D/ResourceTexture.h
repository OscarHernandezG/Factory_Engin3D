#ifndef __ResourceTexture_H__
#define __ResourceTexture_H__

#include "Globals.h"

#include "Resource.h"

class ResourceTexture : public Resource
{
	friend class ModuleResources;
public:
	ResourceTexture(const char* path);
	~ResourceTexture();

	inline uint GetID() { return id; }

protected: 

	inline void SetID(uint id) { this->id = id; }

private:
	uint id = 0;
};

#endif // !__ResourceTexture_H__

