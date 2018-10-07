#include "Mesh.h"
#include "glew-2.1.0/include/GL/glew.h"


void Mesh::InnerRender() const
{
	for (std::vector<MeshBuffer>::const_iterator iterator = buffers.begin(); iterator != buffers.end(); ++iterator)
	{
		glEnableClientState(GL_VERTEX_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, (*iterator).vertex.id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*iterator).index.id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glDrawElements(GL_TRIANGLES, (*iterator).index.size, GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

void Mesh::ClearMesh()
{
	for (std::vector<MeshBuffer>::iterator iterator = buffers.begin(); iterator != buffers.end(); ++iterator)
	{
		glDeleteBuffers(1, (GLuint*)&((*iterator).index.id));
		glDeleteBuffers(1, (GLuint*)&((*iterator).vertex.id));

		delete (*iterator).index.buffer;
		delete (*iterator).vertex.buffer;
	}
}
