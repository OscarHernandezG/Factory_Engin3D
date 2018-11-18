#include "glew-2.1.0/include/GL/glew.h"

#include "ResourceTexture.h"



ResourceTexture::ResourceTexture(const char* path) : Resource(ResourceType::texture, path)
{
}


ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, (GLuint*)&(id));
}
