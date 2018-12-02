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
						glColor4f(1.0f, 1.0f, 1.0f, texture->transparency);
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					//Load Texture UV
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, buffer->texture.id);
					glTexCoordPointer(2, GL_FLOAT, 0, NULL);

					if (texture->haveTransparency)
					{
						glEnable(GL_ALPHA_TEST);
						//glAlphaFunc(GL_GREATER, texture->transparency);
						
					}
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

		//glColor4f(1, 1, 1,);

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
	if (buffer)
	{
		App->resources->Remove(buffer);
	}
}


void Mesh::Inspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (buffer)
		{
			ImGui::Text("Vertices count: %u", buffer->vertex.size);
			ImGui::Text("Faces count: %u", buffer->index.size / 3);

			if (buffer->usage == 1)
				ImGui::Text("Mesh used %i time", buffer->usage);
			else
				ImGui::Text("Mesh used %i times", buffer->usage);

			if (ImGui::Button("Remove Mesh", ImVec2(50, 25)))
			{
				App->resources->Remove(buffer);

				buffer = nullptr;

				if (gameObject)
					gameObject->SetABB(AABB(float3::zero, float3::zero));

				App->gameObject->redoOc = true;
			}
		}
		else
		{
			ImGui::Text("Mesh component has no resource mesh");
			ImGui::Separator();
			if (ImGui::BeginMenu("Add new ResourceMesh"))
			{
				std::vector<Resource*> resource;
				App->resources->GetResources(resource, ResourceType::mesh);

				for (std::vector<Resource*>::iterator iterator = resource.begin(); iterator != resource.end(); ++iterator)
				{
					if (ImGui::MenuItem((*iterator)->name.data()))
					{
						buffer = ((ResourceMesh*)(*iterator));
						App->geometry->LoadBoundingBox(buffer->vertex);
						App->sceneIntro->octree.Insert(gameObject);
					}
				}
				ImGui::End();
			}
			ImGui::Separator();
		}
		if (ImGui::Button("Remove Component", ImVec2(150, 25)))
		{
			toDelete = true;
			gameObject->transform->boundingBox = AABB(float3::zero, float3::zero);
		}
	}
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
