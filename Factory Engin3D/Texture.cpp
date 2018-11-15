#include "Texture.h"

Texture::Texture(GameObject* gameObject, TextureInfo* info) : Component(gameObject)
{
	this->id = info->textureId;
}
