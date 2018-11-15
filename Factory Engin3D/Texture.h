#ifndef __Texture_H__
#define __Texture_H__

#include "Component.h"
#include "GameObject.h"


struct TextureInfo : ComponentInfo
{
	uint textureId = 0;
};

class Texture : Component
{
public:
	Texture(GameObject* gameObject, TextureInfo* info);
	~Texture() {}

	inline uint GetID() { return id; }

private:
	uint id = 0;
};
#endif // !__Texture_H__