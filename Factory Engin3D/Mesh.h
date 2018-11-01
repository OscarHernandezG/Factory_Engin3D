#pragma once

#include "MathGeoLib/MathGeoLib.h"
#include "Geometries.h"

#include "Globals.h"

class Mesh;

struct MeshInfo : public ComponentInfo
{
	Mesh* mesh = nullptr;
};

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
};

class Mesh :public Geometry
{
public:
	Mesh() {};
	Mesh(GameObject* parent) : Geometry(parent) {};
	~Mesh() { ClearMesh(); };

	void InnerRender() const;

	float3 GetPos();

	void ClearMesh();

public:
	std::vector<MeshBuffer> buffers;

};