#include "ParticlePlane.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "GameObject.h"
#include "ResourceTexture.h"

// PARTICLE PLANE ==================================================
ParticlePlane::ParticlePlane()
{
	LoadPlaneBuffers();
}

void ParticlePlane::LoadPlaneBuffers()
{
	float indicesQuad[]
	{
	-0.5f, -0.5f, 0.0f,//a
	 0.5f, -0.5f, 0.0f,//b
	-0.5f,  0.5f, 0.0f,//c
	 0.5f,  0.5f, 0.0f,//d
	};

	glGenBuffers(1, (GLuint*)&(myIndices));
	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, indicesQuad, GL_STATIC_DRAW);
	// 12 = All vertex positions (4 * 3) 4 = posibleVertex and 3 = pos x-y-z
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	uint vertices[]
	{
		// Front
		0, 1, 2, // ABC
		1, 3, 2, // BDC
	};

	glGenBuffers(1, (GLuint*)&(myVertices));
	glBindBuffer(GL_ARRAY_BUFFER, myVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * 6, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	float texture[]
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
	};

	glGenBuffers(1, (GLuint*)&(myTexture));
	glBindBuffer(GL_ARRAY_BUFFER, myTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, texture, GL_STATIC_DRAW);
	//12 = All vertex positions (2 * 6) 2 = vertices and 6 = pos x-y
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticlePlane::Render(float4x4 matrix, ResourceTexture* texture, float4 color) const
{
	glPushMatrix();
	
	glMultMatrixf(matrix.ptr());

	DrawPlane(texture, color);

	glPopMatrix();
}

void ParticlePlane::DrawPlane(ResourceTexture* texture, math::float4 &color) const
{
	//Load vertex and index
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myVertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (texture != nullptr)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Load Texture UV
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, myTexture);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);

		//glColor4f(color.x, color.y, color.z, color.w);
		glEnable(GL_ALPHA_TEST);

		//Load texture
		glBindTexture(GL_TEXTURE_2D, texture->GetID());
	}

	glColor4f(color.x, color.y, color.z, color.w);
	//Draw mesh
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

	//Free buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
