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
	uint id;
	uint size;
	T* buffer;
};

struct MeshBuffer
{
	Buffer<uint> index;
	Buffer<float> vertex;
	Buffer<float> texture;

	string name;
	AABB boundingBox;
};

struct MeshInfo
{
	list<MeshBuffer> meshes;

	string name;

	AABB boundingBox;
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
	MeshBuffer buffer;


};
#endif // !__Mesh_H__