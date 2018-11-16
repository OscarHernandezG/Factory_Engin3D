#include "ResourceMesh.h"


ResourceMesh::ResourceMesh(const char* path) : Resource(ResourceType::mesh, path)
{
}


ResourceMesh::~ResourceMesh()
{
}

bool ResourceMesh::operator==(const ResourceMesh * mesh2) const
{
	return (this->id == mesh2->id);
}
