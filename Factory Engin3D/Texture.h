#ifndef __Texture_H__
#define __Texture_H__

#include "Component.h"
#include "GameObject.h"
#include "ResourceTexture.h"


struct TextureInfo : ComponentInfo
{
	ResourceTexture* texture = nullptr;
};

class Texture : Component
{
public:
	Texture(GameObject* gameObject, TextureInfo* info);
	~Texture() {}

	inline uint GetID() { return texture->GetID(); }

private:
	ResourceTexture* texture = nullptr;

};
#endif // !__Texture_H__