#include "Application.h"

#include "Texture.h"
 

Texture::Texture(GameObject* gameObject, TextureInfo* info) : Component(gameObject)
{
	this->texture = info->texture;
}

void Texture::SaveComponent(JSON_Object * parent)
{
	if (texture)
	{
		json_object_set_number(parent, "Type", type);

		json_object_set_number(parent, "UUID", GetUUID());

		json_object_set_number(parent, "Time Created", GetTime());

		json_object_set_string(parent, "Name", texture->file.data());
	}
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

			ImGui::Button("Remove", ImVec2(64, 64));
		}
		else
		{
			ImGui::Text("No texture loaded");
			ImGui::Button("Load", ImVec2(64, 64));
		}
	}
}
