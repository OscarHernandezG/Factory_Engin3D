#ifndef __Mesh_H__
#define __Mesh_H__

#include "Globals.h"

#include "MathGeoLib/Math/float4.h"
#include "Geometry.h"

#include "ResourceMesh.h"


class Mesh :public Geometry
{
public:
	Mesh() { geoType = PrimitiveTypes::Primitive_Mesh; };
	Mesh(GameObject* parent) : Geometry(parent) { geoType = PrimitiveTypes::Primitive_Mesh; };
	~Mesh() { ClearMesh(); };

	void InnerRender() const;

	float3 GetPos();

	void ClearMesh();

public:
	ResourceMesh* buffer = nullptr;

	bool hasColor = false;
	float4 color = float4::zero;

	int meshId;
};


class MeshNode
{
public:
	MeshNode() {}
	~MeshNode() {}

	bool operator==(const MeshNode& node2) const;

	bool operator!=(const MeshNode& node2) const;

	bool operator<(const MeshNode& node2) const;

public:
	std::string name;

	int id = -1;

	uint componentUUID = 0;

	ResourceMesh* buffer;
	std::list<MeshNode> childs;

	float4x4 transform;

	bool hasTexture = false;
	int textureId = 0;

	bool hasColor = false;
	float4 color = float4::zero;


};

#endif // !__Mesh_H__