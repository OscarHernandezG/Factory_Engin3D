#include "Globals.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Primitive.h"

#include "MathGeoLib/Math/TransformOps.h"
#include "MathGeoLib/Math/MathConstants.h"


// ------------------------------------------------------------
Primitive::Primitive() : transform(float4x4::identity), color(White), wire(false), axis(false), fill(true), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
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
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::WireframeRender() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0, 1, 0);

	InnerRender();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1, 1, 1);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform[3][0] = x;
	transform[3][1] = y;
	transform[3][2] = z;
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const float3 &u)
{
	transform = float4x4::RotateAxisAngle(u, angle) * transform;
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform = float4x4::Scale(x, y, z).ToFloat4x4() * transform;
}

// CUBE ============================================
PrimitiveCube::PrimitiveCube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;

	float indicesQuad[]
	{
	-0.5f, -0.5f, -0.5f,//a
	 0.5f, -0.5f, -0.5f,//b
	-0.5f,  0.5f, -0.5f,//c
	 0.5f,  0.5f, -0.5f,//d
	-0.5f, -0.5f,  0.5f,//e
	 0.5f, -0.5f,  0.5f,//f
	-0.5f,  0.5f,  0.5f,//g
	 0.5f,  0.5f,  0.5f,//h
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

}

PrimitiveCube::PrimitiveCube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;

	float indicesQuad[]
	{
	sizeX * -0.5f, sizeY * -0.5f, sizeZ * -0.5f,//a
	sizeX *  0.5f, sizeY * -0.5f, sizeZ * -0.5f,//b
	sizeX * -0.5f, sizeY *  0.5f, sizeZ * -0.5f,//c
	sizeX *  0.5f, sizeY *  0.5f, sizeZ * -0.5f,//d
	sizeX * -0.5f, sizeY * -0.5f, sizeZ *  0.5f,//e
	sizeX *  0.5f, sizeY * -0.5f, sizeZ *  0.5f,//f
	sizeX * -0.5f, sizeY *  0.5f, sizeZ *  0.5f,//g
	sizeX *  0.5f, sizeY *  0.5f, sizeZ *  0.5f,//h
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
//Sphere::Sphere() : Primitive(), radius(1.0f)
//{
//	type = PrimitiveTypes::Primitive_Sphere;
//}
//
//Sphere::Sphere(float radius) : Primitive(), radius(radius)
//{
//	type = PrimitiveTypes::Primitive_Sphere;
//}
//
//void Sphere::InnerRender() const
//{
//	glutSolidSphere(radius, 25, 25);
//}
//
//
//// CYLINDER ============================================
//Cylinder::Cylinder() : Primitive(), radius(1.0f), height(1.0f)
//{
//	type = PrimitiveTypes::Primitive_Cylinder;
//}
//
//Cylinder::Cylinder(float radius, float height) : Primitive(), radius(radius), height(height)
//{
//	type = PrimitiveTypes::Primitive_Cylinder;
//}
//
//void Cylinder::InnerRender() const
//{
//	int n = 30;
//
//	// Cylinder Bottom
//	glBegin(GL_POLYGON);
//	
//	for(int i = 360; i >= 0; i -= (360 / n))
//	{
//		float a = i * M_PI / 180; // degrees to radians
//		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
//	}
//	glEnd();
//
//	// Cylinder Top
//	glBegin(GL_POLYGON);
//	glNormal3f(0.0f, 0.0f, 1.0f);
//	for(int i = 0; i <= 360; i += (360 / n))
//	{
//		float a = i * M_PI / 180; // degrees to radians
//		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
//	}
//	glEnd();
//
//	// Cylinder "Cover"
//	glBegin(GL_QUAD_STRIP);
//	for(int i = 0; i < 480; i += (360 / n))
//	{
//		float a = i * M_PI / 180; // degrees to radians
//
//		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
//		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
//	}
//	glEnd();
//}
//
// LINE ==================================================
RayLine::RayLine() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	
	type = PrimitiveTypes::Primitive_Ray;
}

RayLine::RayLine(float3 origin, float3 destination) : Primitive(), origin(origin), destination(destination)
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
PrimitivePlane::PrimitivePlane() : Primitive(), normal(0.0f, 1.0f, 0.0f), constant(1.0f)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PrimitivePlane::PrimitivePlane(float3 normal, float d) : Primitive(), normal(normal), constant(d)
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