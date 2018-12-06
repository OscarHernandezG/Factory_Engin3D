#include "ParticlePlane.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "GameObject.h"
#include "Texture.h"

// PARTICLE PLANE ==================================================
ParticlePlane::ParticlePlane() : Geometry()
{
	LoadPlaneBuffers(float3::zero);
	geoType = PrimitiveTypes::Primitive_Plane;
}

ParticlePlane::ParticlePlane(float3 position) : Geometry()
{
	LoadPlaneBuffers(position);
	geoType = PrimitiveTypes::Primitive_Plane;
}

void ParticlePlane::LoadPlaneBuffers(float3 position)
{
	float indicesQuad[]
	{
	position.x + -0.5f, position.y, position.z + -0.5f, //a
	position.x +  0.5f, position.y, position.z + -0.5f, //b
	position.x +  0.5f, position.y, position.z +  0.5f, //c
	position.x + -0.5f, position.y, position.z +  0.5f, //d
	};

	glGenBuffers(1, (GLuint*)&(myIndices));
	glBindBuffer(GL_ARRAY_BUFFER, myIndices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, indicesQuad, GL_STATIC_DRAW);
	// 24 = All vertex positions (4 * 3) 4 = posibleVertex and 3 = pos x-y-z
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint vertices[]
	{
		0, 1, 2, // ABC
		1, 2, 3, // BCD
	};

	glGenBuffers(1, (GLuint*)&(myVertices));
	glBindBuffer(GL_ARRAY_BUFFER, myVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * 8, vertices, GL_STATIC_DRAW);
	// 6 = All vertex positions (2 * 3) 2 = vertices and 3 = pos x-y-z
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	float texture[]
	{

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	glGenBuffers(1, (GLuint*)&(myTexture));
	glBindBuffer(GL_ARRAY_BUFFER, myTexture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, texture, GL_STATIC_DRAW);
	// 12 = All vertex positions (2 * 6) 2 = vertices and 6 = pos x-y-z
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ParticlePlane::InnerRender() const
{
	bool hasTexture = gameObject->HasComponent(ComponentType_TEXTURE);

	//Load vertex and index
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, myVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myIndices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (hasTexture)
	{
		Texture* texture = (Texture*)gameObject->GetComponent(ComponentType_TEXTURE);
		if (texture != nullptr)
		{

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//Load Texture UV
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, myTexture);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);

			if (texture->haveTransparency)
			{
				glColor4f(1.0f, 1.0f, 1.0f, texture->transparency);
				glEnable(GL_ALPHA_TEST);
			}
			//Load texture
			glBindTexture(GL_TEXTURE_2D, texture->GetID());
		}
	}

	//Draw mesh
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, NULL); //12 = number of indices

	//Free buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}
