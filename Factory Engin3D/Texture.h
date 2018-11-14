#ifndef __Texture_H__
#define __Texture_H__

#include "Component.h"
#include "GameObject.h"


struct TextureyInfo : ComponentInfo
{
	uint textureId = 0;
};

class Texture : Component
{
public:
	Texture(GameObject* gameObject, TextureyInfo* info);
	~Texture() {}

	inline uint GetID() { return id; }

private:
	uint id = 0;
};
#endif // !__Texture_H__