#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateGameTime()
{
	if (ImGui::Begin("GameSimulate", &gameTimeWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
	{
		if (ImGui::Button("Play", { 50,25 }))
			App->time->gameState = GameState_PLAYING;
		
		ImGui::SameLine();

		if (ImGui::Button("Pause", { 50,25 }))
				App->time->gameState = GameState_PAUSE;

		ImGui::SameLine();

		if (ImGui::Button("Stop", { 50,25 }))
		{
			App->time->gameState = GameState_STOP;
		}

		ImGui::Text("Game timer: ");

		ImGui::SameLine();
		std::string gameCount = std::to_string(App->time->GetGameTimer());
		ImGui::Text(gameCount.data());

	}
	
	ImGui::End();

}
