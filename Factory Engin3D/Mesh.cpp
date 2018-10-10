#include "Mesh.h"
#include "glew-2.1.0/include/GL/glew.h"
#include "Application.h"
#include "ModuleGeometryManager.h"

void Mesh::InnerRender() const
{
	for (std::vector<MeshBuffer>::const_iterator iterator = buffers.begin(); iterator != buffers.end(); ++iterator)
	{
		//Load vertex and index
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, (*iterator).vertex.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*iterator).index.id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		
		//Load Texture UV
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, (*iterator).texture.id);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		
		//Load texture
		glBindTexture(GL_TEXTURE_2D, App->geometry->imageID);

		//Draw mesh
		glDrawElements(GL_TRIANGLES, (*iterator).index.size, GL_UNSIGNED_INT, NULL);

		//Free buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);	
	}
}

void Mesh::ClearMesh()
{
	//for (std::vector<MeshBuffer>::iterator iterator = buffers.begin(); iterator != buffers.end(); ++iterator)
	//{
	//	glDeleteBuffers(1, (GLuint*)&((*iterator).index.id));
	//	glDeleteBuffers(1, (GLuint*)&((*iterator).vertex.id));

	//	delete (*iterator).index.buffer;
	//	delete (*iterator).vertex.buffer;
	//}
}
