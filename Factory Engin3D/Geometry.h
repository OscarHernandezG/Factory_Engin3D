#ifndef __ModuleGeometry_H__
#define __ModuleGeometry_H__
#include "Globals.h"

#include "MathGeoLib/MathGeoLib.h"
#include "Color.h"

#include "Component.h"

class Transform;
class Geometry;

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Ray,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Frustum,
	Primitive_Mesh
};

struct GeometryInfo : ComponentInfo
{
	GeometryInfo() {}
	GeometryInfo(Geometry* prim) : geometry(prim) {}

	Geometry* geometry = nullptr;
	AABB boundingBox;
};

class Geometry : public Component
{
public:
	Geometry();
	Geometry(GameObject* parent);

	virtual void	Render() const;
	virtual void	InnerRender() const;

	//virtual void	Update(float dt) { Render(); }

	void			WireframeRender() const;
	void SaveComponent(JSON_Object * parent);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	bool axis = false, wire = false, fill = true;

protected:
	PrimitiveTypes geoType;
};

// ============================================
class PrimitiveCube : public Geometry
{
public :
	PrimitiveCube();
	PrimitiveCube(float3 position, float sizeX, float sizeY, float sizeZ);

	void LoadCubeBuffers(float3 position, float sizeX, float sizeY, float sizeZ);
	void LoadCubeBuffers(float3 position, float3 size)
	{
		LoadCubeBuffers(position, size.x, size.y, size.z);
	}

	~PrimitiveCube();

	void InnerRender() const;

public:
	float3 size;
	uint myIndices = 0u;
	uint myVertices = 0u;
};

//// ============================================
class PrimitiveSphere : public Geometry
{
public:
	PrimitiveSphere();
	PrimitiveSphere(float radius, int faces);
	void InnerRender() const;
public:
	float radius;
	int faces;
};

//// ============================================
class PrimitiveCylinder : public Geometry
{
public:
	PrimitiveCylinder();
	PrimitiveCylinder(float radius, float height, int faces);
	void InnerRender() const;
public:
	float radius;
	float height;
	int faces;
};

class RayLine : public Geometry
{
public:
	RayLine();
	RayLine(float3 origin, float3 destination);
	void InnerRender() const;
public:
	float3 origin;
	float3 destination;
};

// ============================================
class PrimitivePlane : public Geometry
{
public:
	PrimitivePlane();
	PrimitivePlane(float3 normal, float d);
	void InnerRender() const;
public:
	float3 normal;
	float constant;
};

// ============================================
class PrimitiveFrustum : public Geometry
{
public:
	PrimitiveFrustum();
	PrimitiveFrustum(float highSizes, float lowSize, float3 position = { 0,0,0 }, float sizeX = 1, float sizeY = 1, float sizeZ = 1);
	~PrimitiveFrustum();

	void LoadFrustumBuffers(float highSizes = 1, float lowSize = 1, float3 position = { 0,0,0 }, float sizeX = 1, float sizeY = 1, float sizeZ = 1);

	void InnerRender() const;

public:
	float3 size;
	uint myIndices = 0u;
	uint myVertices = 0u;

};
#endif // !__ModuleGeometry_H__
