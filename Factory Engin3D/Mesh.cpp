#include "Mesh.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Application.h"
#include "ModuleGeometry.h"

#include "GameObject.h"

void Mesh::InnerRender() const
{
	if (buffer != nullptr)
	{
		//Load vertex and index
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, buffer->vertex.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->index.id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		//Load Texture UV
		if (buffer->texture.buffer != nullptr && buffer->texture.size > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, buffer->texture.id);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);

			//Load texture
			glBindTexture(GL_TEXTURE_2D, App->geometry->textureID);
		}

		//Draw mesh
		glDrawElements(GL_TRIANGLES, buffer->index.size, GL_UNSIGNED_INT, NULL);

		//Free buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	//		delete (*iterator).texture.buffer;
}

