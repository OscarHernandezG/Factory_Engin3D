/*
#pragma once
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/float3.h"
#include "glmath.h"
#include "MathGeoLib/src/Geometry/GeometryAll.h"

#include "Color.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

enum Geomtype;
class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const float3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	float4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	float3 size;
};

// ============================================
class Spheree : public Primitive
{
public:
	Spheree();
	Spheree(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class Cylinder : public Primitive
{
public:
	Cylinder();
	Cylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class Linee : public Primitive
{
public:
	Linee();
	Linee(float x, float y, float z);
	void InnerRender() const;
public:
	float3 origin;
	float3 destination;
};

// ============================================
class Plane : public Primitive
{
public:
	Plane();
	Plane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	float3 normal;
	float constant;
};*/