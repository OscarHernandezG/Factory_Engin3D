#include "Texture.h"

Texture::Texture(GameObject* gameObject, TextureyInfo* info) : Component(gameObject)
{
	this->id = info->textureId;
}
