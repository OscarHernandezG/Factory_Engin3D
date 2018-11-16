#include "Mesh.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Application.h"
#include "ModuleGeometry.h"
#include "Texture.h"

#include "GameObject.h"

void Mesh::InnerRender() const
{
	if (buffer != nullptr)
	{
		bool hasTexture = gameObject->HasComponent(ComponentType_TEXTURE);

		//Load vertex and index
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, buffer->vertex.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->index.id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		if (hasTexture)
		{
			if (buffer->texture.buffer != nullptr && buffer->texture.size > 0)
			{
				Texture* texture = (Texture*)gameObject->GetComponent(ComponentType_TEXTURE);
				if (texture != nullptr)
				{
					//Load Texture UV
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, buffer->texture.id);
					glTexCoordPointer(2, GL_FLOAT, 0, NULL);

					//Load texture
					glBindTexture(GL_TEXTURE_2D, texture->GetID());
				}
			}
		}

		if (buffer->color.buffer != nullptr)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer->color.id);
			glColorPointer(4, GL_FLOAT, 0, NULL);
		}

		//Draw mesh
		glDrawElements(GL_TRIANGLES, buffer->index.size, GL_UNSIGNED_INT, NULL);

		//Free buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//glColor4f(1, 1, 1, 1);

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

float3 Mesh::GetPos()
{
	if (gameObject != nullptr)
	{
		return gameObject->GetPos();
	}
	return float3::zero;
}

void Mesh::ClearMesh()
{
	glDeleteBuffers(1, (GLuint*)&(buffer->index.id));
	glDeleteBuffers(1, (GLuint*)&(buffer->vertex.id));
	glDeleteBuffers(1, (GLuint*)&(buffer->texture.id));


	delete buffer->index.buffer;
	delete buffer->vertex.buffer;
	delete buffer->texture.buffer;
}




// ------------------------------------------------------------------------------
// Mesh Node
// ------------------------------------------------------------------------------
bool MeshNode::operator==(const MeshNode& node2) const
{
	return (this->id == node2.id);
}

bool MeshNode::operator!=(const MeshNode& node2) const
{
	return !(this->id == node2.id);
}

bool MeshNode::operator<(const MeshNode& node2) const
{
	return (this->id < node2.id);
}
