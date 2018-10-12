#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Geometry.h"

#include "MathGeoLib/Math/TransformOps.h"
#include "MathGeoLib/Math/MathConstants.h"


// ------------------------------------------------------------
Geometry::Geometry() : transform(float4x4::identity), color(White), wire(false), axis(false), fill(true), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Geometry::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Geometry::Render() const
{
	glPushMatrix();
	glMultMatrixf((GLfloat*)transform.ptr());

	if (axis)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if (fill)
		InnerRender();

	if (wire)
		WireframeRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Geometry::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Geometry::WireframeRender() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 1, 0);

	InnerRender();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1, 1, 1);
}

// ------------------------------------------------------------
void Geometry::SetPos(float x, float y, float z)
{
	transform[3][0] = x;
	transform[3][1] = y;
	transform[3][2] = z;
}

// ------------------------------------------------------------
void Geometry::SetRotation(float angle, const float3 &u)
{
	transform = float4x4::RotateAxisAngle(u, angle) * transform;
}

// ------------------------------------------------------------

void Geometry::SetScale(float x, float y, float z)
{
	float4x4 initialScale = float4x4::identity;
	//SetPos
	initialScale[3][0] = transform[3][0];
	initialScale[3][1] = transform[3][1];
	initialScale[3][2] = transform[3][2];
	transform = float4x4::Scale(x, y, z).ToFloat4x4() * initialScale;
}

void Geometry::SetIdentity()
{
	transform = float4x4::identity;
}

float3 Geometry::GetPos() const
{
	return { transform[3][0], transform[3][1], transform[3][2] };
}

/*float3 Primitive::GetScale() const
{ 
	return { transform[0][0], transform[1][1], transform[2][2] };
}*/

/*float3 Primitive::GetRotation() const
{
	float3x3 rot = transform.RotatePart();

	float3 angles = rot.ToEulerXYX();
	math::RadToDeg(angles.x);
	math::RadToDeg(angles.y);
	math::RadToDeg(angles.z);

	return angles;
}*/

// CUBE ============================================
PrimitiveCube::PrimitiveCube() : Geometry(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
	LoadCubeBuffers({ 0,0,0 },size);
}

PrimitiveCube::PrimitiveCube(float3 position, float sizeX, float sizeY, float sizeZ) : Geometry(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;

	LoadCubeBuffers(position, sizeX, sizeY, sizeZ);
}


void PrimitiveCube::LoadCubeBuffers(float3 position, float sizeX, float sizeY, float sizeZ)
{
	float indicesQuad[]
	{
	position.x + (sizeX * -0.5f), position.y + (sizeY * -0.5f), position.z + (sizeZ * -0.5f), //a
	position.x + (sizeX *  0.5f), position.y + (sizeY * -0.5f), position.z + (sizeZ * -0.5f), //b
	position.x + (sizeX * -0.5f), position.y + (sizeY *  0.5f), position.z + (sizeZ * -0.5f), //c
	position.x + (sizeX *  0.5f), position.y + (sizeY *  0.5f), position.z + (sizeZ * -0.5f), //d
	position.x + (sizeX * -0.5f), position.y + (sizeY * -0.5f), position.z + (sizeZ *  0.5f), //e
	position.x + (sizeX *  0.5f), position.y + (sizeY * -0.5f), position.z + (sizeZ *  0.5f), //f
	position.x + (sizeX * -0.5f), position.y + (sizeY *  0.5f), position.z + (sizeZ *  0.5f), //g
	position.x + (sizeX *  0.5f), position.y + (sizeY *  0.5f), position.z + (sizeZ *  0.5f), //h
	};

	glGenBuffers(1, (GLuint*)&(myIndices));
	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, indicesQuad, GL_STATIC_DRAW);
	// 24 = All vertex positions (8 * 3) 8 = posibleVertex and 3 = pos x-y-z
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint vertices[]
	{
		// Front
		0, 1, 2, // ABC
		1, 3, 2, // BDC

		// Right
		1, 5, 3, // BFD
		5, 7, 3, // FHD

		// Back
		5, 4, 7, // FEH
		4, 6, 7, // EGH

		// Left
		4, 0, 6, // EAG
		0, 2, 6, // ACG

		// Top
		2, 3, 6, // CDG
		3, 7, 6, // DHG

		// Bottom
		0, 4, 1, // AEB
		1, 4, 5  // BEF
	};

	glGenBuffers(1, (GLuint*)&(myVertices));
	glBindBuffer(GL_ARRAY_BUFFER, myVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * 36, vertices, GL_STATIC_DRAW);
	// 36 = All vertex positions (12 * 3) 12 = vertices and 3 = pos x-y-z
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


PrimitiveCube::~PrimitiveCube()
{
	glDeleteBuffers(1, (GLuint*)&(myIndices));
	glDeleteBuffers(1, (GLuint*)&(myVertices));
}

void PrimitiveCube::InnerRender() const
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myVertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}

// SPHERE ============================================
SpherePrim::SpherePrim() : Geometry(), radius(1.0f), horizontalCuts(21), verticalCuts(10)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

SpherePrim::SpherePrim(float radius, int horizontalCuts, int verticalCuts) : Geometry(), radius(radius), horizontalCuts(horizontalCuts), verticalCuts(verticalCuts)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void SpherePrim::InnerRender() const
{
	glBegin(GL_TRIANGLES);

	float currentRad = radius;
	int verticalDeg = 360 / verticalCuts;
	float intervals = radius/(horizontalCuts/2);
	for (int horizontalCont = horizontalCuts / 2; horizontalCont > 0; --horizontalCont)
	{
		float newRad = currentRad - intervals;
		
		for (int i = 0; i < 360; i += verticalDeg)
		{
			float a = i * PI / 180; // degrees to radians
			glVertex3f(currentRad * cos(a) + pow((currentRad * (radius-currentRad)), 2), radius - currentRad, currentRad * sin(a) + pow((currentRad * (radius - currentRad)), 2));
			glVertex3f(newRad * cos(a) + pow((newRad * (radius - newRad)), 2), radius - newRad, newRad * sin(a) + pow((newRad * (radius - newRad)), 2));
			a = (i + verticalDeg) * PI / 180; // degrees to radians
			glVertex3f(currentRad * cos(a) + pow((currentRad * (radius - currentRad)), 2), radius - currentRad, currentRad * sin(a) + pow((currentRad * (radius - currentRad)), 2));

			glVertex3f(newRad * cos(a) + pow((newRad * (radius - newRad)), 2), radius - newRad, newRad * sin(a) + pow((newRad * (radius - newRad)), 2));
			glVertex3f(currentRad * cos(a) + pow((currentRad * (radius - currentRad)), 2), radius - currentRad, currentRad * sin(a) + pow((currentRad * (radius - currentRad)), 2));
			a = i * PI / 180; // degrees to radians
			glVertex3f(newRad * cos(a) + pow((newRad * (radius - newRad)), 2), radius - newRad, newRad * sin(a) + pow((newRad * (radius - newRad)), 2));
		}
		currentRad = newRad;
}
	glEnd();

}


// CYLINDER ============================================
PrimitiveCylinder::PrimitiveCylinder() : Geometry(), radius(1.0f), height(1.0f), faces(40)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

PrimitiveCylinder::PrimitiveCylinder(float radius, float height, int faces) : Geometry(), radius(radius), height(height), faces(faces)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void PrimitiveCylinder::InnerRender() const
{
	// Cylinder Bottom
	glBegin(GL_TRIANGLES);
	
	int deg = 360 / faces;
	for(int i = 0; i < 360; i += deg)
	{
		float a = i * PI / 180; // degrees to radians
		glVertex3f(0, -height * 0.5f, 0);
		glVertex3f(radius * cos(a), -height * 0.5f, radius * sin(a));
		a = (i + deg)* PI / 180; // degrees to radians
		glVertex3f(radius * cos(a), -height * 0.5f, radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_TRIANGLES);
	for (int i = 360; i > 0; i -= deg)
	{
		float a = i * PI / 180; // degrees to radians
		glVertex3f(0, height * 0.5f, 0);
		glVertex3f(radius * cos(a), height * 0.5f, radius * sin(a));
		a = (i - deg)* PI / 180; // degrees to radians
		glVertex3f(radius * cos(a), height * 0.5f, radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_TRIANGLES);
	for(int i = 0; i < 360; i += deg)
	{
		float a = i * PI / 180; // degrees to radians

		glVertex3f(radius * cos(a), height * 0.5f, radius * sin(a));
		glVertex3f(radius * cos(a), -height * 0.5f, radius * sin(a));
		a = (i - deg)* PI / 180; // degrees to radians
		glVertex3f(radius * cos(a), height * 0.5f, radius * sin(a));
		//next Triangle for make a square

		glVertex3f(radius * cos(a), -height * 0.5f, radius * sin(a));
		glVertex3f(radius * cos(a), height * 0.5f, radius * sin(a));
		a = (i)* PI / 180; // degrees to radians
		glVertex3f(radius * cos(a), -height * 0.5f, radius * sin(a));
	}
	glEnd();
}

RayLine::RayLine() : Geometry(), origin(0, 0, 0), destination(1, 1, 1)
{
	
	type = PrimitiveTypes::Primitive_Ray;
}

RayLine::RayLine(float3 origin, float3 destination) : Geometry(), origin(origin), destination(destination)
{
	type = PrimitiveTypes::Primitive_Ray;
}

void RayLine::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
PrimitivePlane::PrimitivePlane() : Geometry(), normal(0.0f, 1.0f, 0.0f), constant(1.0f)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PrimitivePlane::PrimitivePlane(float3 normal, float d) : Geometry(), normal(normal), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void PrimitivePlane::InnerRender() const
{
	glBegin(GL_LINES);

	float d = 200.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}


// Frustum ================================================
PrimitiveFrustum::PrimitiveFrustum() : Geometry()
{
	type = PrimitiveTypes::Primitive_Plane;
	LoadFrustumBuffers();
}

PrimitiveFrustum::PrimitiveFrustum(float highSizes, float lowSize, float3 position, float sizeX, float sizeY, float sizeZ)
{
	type = PrimitiveTypes::Primitive_Frustum;
	LoadFrustumBuffers(highSizes, lowSize, position, sizeX, sizeY, sizeZ);

}
PrimitiveFrustum::~PrimitiveFrustum()
{
	glDeleteBuffers(1, (GLuint*)&(myIndices));
	glDeleteBuffers(1, (GLuint*)&(myVertices));
}
void PrimitiveFrustum::LoadFrustumBuffers(float hSize, float lSize, float3 position, float sizeX, float sizeY, float sizeZ)
{
	float indicesQuad[]
	{
	position.x + (sizeX * -0.5f * lSize),	position.y + (sizeY * -0.5f), position.z + (sizeZ * -0.5f * lSize),//a
	position.x + (sizeX *  0.5f * lSize),	position.y + (sizeY * -0.5f), position.z + (sizeZ * -0.5f * lSize),//b
	position.x + (sizeX * -0.5f * hSize),	position.y + (sizeY *  0.5f), position.z + (sizeZ * -0.5f * hSize),//c
	position.x + (sizeX *  0.5f * hSize),	position.y + (sizeY *  0.5f), position.z + (sizeZ * -0.5f * hSize),//d
	position.x + (sizeX * -0.5f * lSize),	position.y + (sizeY * -0.5f), position.z + (sizeZ *  0.5f * lSize),//e
	position.x + (sizeX *  0.5f * lSize),	position.y + (sizeY * -0.5f), position.z + (sizeZ *  0.5f * lSize),//f
	position.x + (sizeX * -0.5f * hSize),	position.y + (sizeY *  0.5f), position.z + (sizeZ *  0.5f * hSize),//g
	position.x + (sizeX *  0.5f * hSize),	position.y + (sizeY *  0.5f), position.z + (sizeZ *  0.5f * hSize),//h
	};

	glGenBuffers(1, (GLuint*)&(myIndices));
	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, indicesQuad, GL_STATIC_DRAW);
	// 24 = All vertex positions (8 * 3) 8 = posibleVertex and 3 = pos x-y-z

	uint vertices[]
	{
		// Front
		0, 1, 2, // ABC
		1, 3, 2, // BDC

		// Right
		1, 5, 3, // BFD
		5, 7, 3, // FHD

		// Back
		5, 4, 7, // FEH
		4, 6, 7, // EGH

		// Left
		4, 0, 6, // EAG
		0, 2, 6, // ACG

		// Top
		2, 3, 6, // CDG
		3, 7, 6, // DHG

		// Bottom
		0, 4, 1, // AEB
		1, 4, 5  // BEF
	};

	glGenBuffers(1, (GLuint*)&(myVertices));
	glBindBuffer(GL_ARRAY_BUFFER, myVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * 36, vertices, GL_STATIC_DRAW);
	// 36 = All vertex positions (12 * 3) 12 = vertices and 3 = pos x-y-z

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PrimitiveFrustum::InnerRender() const
{
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myVertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}