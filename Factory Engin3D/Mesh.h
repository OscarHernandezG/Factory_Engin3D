#pragma once

#include "MathGeoLib/MathGeoLib.h"
#include "Primitive.h"

#include "Globals.h"

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
};

class Mesh :public Primitive
{
public:
	Mesh() {};
	~Mesh() { ClearMesh(); };

	void InnerRender() const;

	void ClearMesh();

public:
	std::vector<MeshBuffer> buffers;

};