#include "Application.h"

#include "ComponentTexture.h"
 

Texture::Texture(GameObject* gameObject, TextureInfo* info) : Component(gameObject)
{
	this->texture = info->texture;
}

Texture::~Texture()
{
	if (texture)
		App->resources->Remove(texture);

	texture = nullptr;
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

			ImGui::Image((void*)(intptr_t)texture->GetID(), ImVec2(256, 256), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

			ImGui::Checkbox("Transparency", &haveTransparency);
			if (haveTransparency)
			{
				ImGui::SliderFloat(" ", &transparency, 0.0f, 1.0f);
			}

			if (ImGui::BeginMenu("Change Texture"))
			{
				std::vector<Resource*> resource;
				App->resources->GetResources(resource, ResourceType::texture);

				for (std::vector<Resource*>::iterator iterator = resource.begin(); iterator != resource.end(); ++iterator)
				{
					if (ImGui::MenuItem((*iterator)->name.data()))
					{
						App->resources->Remove(texture);
						texture = nullptr;

						texture = ((ResourceTexture*)(*iterator));
						texture->usage++;
					}
				}
				ImGui::End();
			}

			if (ImGui::Button("Remove", ImVec2(50, 25)))
			{
				App->resources->Remove(texture);
				texture = nullptr;
			}
		}
		else
		{
			ImGui::Text("No texture loaded");
			if (ImGui::BeginMenu("Add new Texture"))
			{
				std::vector<Resource*> resource;
				App->resources->GetResources(resource, ResourceType::texture);

				for (std::vector<Resource*>::iterator iterator = resource.begin(); iterator != resource.end(); ++iterator)
				{
					if (ImGui::MenuItem((*iterator)->name.data()))
					{
						texture = ((ResourceTexture*)(*iterator));
						texture->usage++;
					}
				}
				ImGui::End();
			}
			ImGui::Separator();
		}
		if (ImGui::Button("Remove Component", ImVec2(150, 25)))
			toDelete = true;
	}
}
