#include "glew-2.1.0/include/GL/glew.h"

#include "ResourceMesh.h"


ResourceMesh::ResourceMesh(const char* path) : Resource(ResourceType::mesh, path)
{
}


ResourceMesh::~ResourceMesh()
{
	glDeleteBuffers(1, (GLuint*)&(index.id));
	glDeleteBuffers(1, (GLuint*)&(vertex.id));
	glDeleteBuffers(1, (GLuint*)&(texture.id));
	glDeleteBuffers(1, (GLuint*)&(color.id));
}

bool ResourceMesh::operator==(const ResourceMesh * mesh2) const
{
	return (this->id == mesh2->id);
}
