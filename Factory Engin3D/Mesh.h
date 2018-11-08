#ifndef __Mesh_H__
#define __Mesh_H__

#include "MathGeoLib/MathGeoLib.h"
#include "Geometries.h"

#include "Globals.h"

#include <list>
#include "MathGeoLib/Geometry/AABB.h"

using namespace std;


template <typename T>
struct Buffer
{
	uint id = 0u;
	uint size = 0u;
	T* buffer = nullptr;
};

struct MeshBuffer
{
	Buffer<uint> index;
	Buffer<float> vertex;
	Buffer<float> texture;

	AABB boundingBox;

	int id = -1;

	bool operator ==(MeshBuffer* mesh2)
	{
		return (this->id == mesh2->id);
	}
};

struct MeshNode
{
	string name;
	
	int id = -1;
	
	MeshBuffer buffer;
	list<MeshNode> childs;

	float4x4 transform;

	bool operator ==(MeshNode node2)
	{
		return (this->id == node2.id);
	}
	bool operator !=(MeshNode node2)
	{
		return !(this->id == node2.id);
	}

	bool operator <(MeshNode node2)
	{
		return (this->id < node2.id);
	}
};

class Mesh :public Geometry
{
public:
	Mesh() { type = PrimitiveTypes::Primitive_Mesh; };
	Mesh(GameObject* parent) : Geometry(parent) { type = PrimitiveTypes::Primitive_Mesh; };
	~Mesh() { ClearMesh(); };

	void InnerRender() const;

	float3 GetPos();

	void ClearMesh();

public:
	MeshBuffer* buffer;

	int meshId;
};
#endif // !__Mesh_H__