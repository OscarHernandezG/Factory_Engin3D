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

	~Texture();

	void SaveComponent(JSON_Object * parent);
	void Inspector();

	inline uint GetID() { if (texture) return texture->GetID(); else return 0u; }

private:
	ResourceTexture* texture = nullptr;

	std::string name;
};
#endif // !__Texture_H__