#ifndef __ResourceMesh_H__
#define __ResourceMesh_H__

#include "Globals.h"

#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

#include "MathGeoLib/Geometry/AABB.h"

#include "Resource.h"

#include <list>

template <typename T>
struct Buffer
{
	uint id = 0u;
	uint size = 0u;
	T* buffer = nullptr;
};

class ResourceMesh : public Resource
{
public:
	ResourceMesh() {}
	ResourceMesh(const char* path);
	~ResourceMesh();

	bool operator==(const ResourceMesh* mesh2) const;

public:
	Buffer<uint> index;
	Buffer<float> vertex;
	Buffer<float> texture;
	Buffer<float> color;

	AABB boundingBox = AABB(-float3::one / 2, float3::one / 2);

	int id = -1;

	uint uuid = 0;

};
#endif // !__ResourceMesh_H__
