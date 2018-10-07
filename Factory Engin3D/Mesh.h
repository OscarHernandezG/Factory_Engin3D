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
	~Mesh(){};

	void InnerRender() const;

public:

//	uint idIndex = 0u;
//	uint  numIndex = 0u;
//	uint* index = nullptr;

	std::vector<MeshBuffer> buffers;

//	uint idVertex = 0u;
//	uint numVertex = 0u;
//	float* vertex = nullptr;


};