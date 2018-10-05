#pragma once

#include "MathGeoLib/MathGeoLib.h"
#include "Primitive.h"

#include "Globals.h"

class Mesh :public Primitive
{
public:
	Mesh();
	~Mesh();


public:

	uint idIndex = 0u;
	uint  numIndex = 0u;
	uint* index = nullptr;

	uint idVertex = 0u;
	uint numVertex = 0u;
	float* vertex = nullptr;
};