#include "Application.h"

#include "Texture.h"
 

Texture::Texture(GameObject* gameObject, TextureInfo* info) : Component(gameObject)
{
	this->texture = info->texture;
}

void Texture::Inspector()
{
	if (ImGui::CollapsingHeader("Texture", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (texture)
		{
			name = texture->file;
			name = name.substr(name.find_last_of("\\") + 1);

			ImGui::Text("Loaded texture '%s'", name.data());
			ImGui::Text("Texture used %i times", texture->usage);

			ImGui::Image((void*)(intptr_t)texture->GetID(), ImVec2(256, 256));
		}
		else
		{
			ImGui::Text("No texture loaded");
		}
	}
}
