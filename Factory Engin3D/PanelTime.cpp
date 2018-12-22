#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleParticles.h"

void ModuleImGui::CreateGameManager(float2 scale)
{
	if (ImGui::Begin("PlayingButtons", &canScroll, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		SetWindowDim(playPos, playSize, scale, true);

		std::string btnName;
		if (App->time->gameState == GameState_NONE)
			btnName = "Play";
		else
			btnName = "Stop";

		if (ImGui::Button(btnName.data(), { 50,25 }))
		{
			if (App->time->gameState == GameState_NONE)
				ChangePlayState(false, GameState_PLAYING);
			else
				ChangePlayState(true, GameState_STOP);

		}
		ImGui::SameLine();

		if (ImGui::Button("Pause", { 50,25 }))
		{
			if (App->time->gameState == GameState_PLAYING)
				App->time->gameState = GameState_PAUSE;
			else if(App->time->gameState != GameState_NONE)
				App->time->gameState = GameState_PLAYING;
		}
		ImGui::SameLine();

		if (ImGui::Button("Tick", { 50,25 }))
		{
			if (App->time->gameState != GameState_NONE)
				App->time->gameState = GameState_TICK;
		}

		if (App->time->gameState != GameState_NONE)
		{
			float scale = App->time->GetGameScale();
			ImGui::SameLine();
			ImGui::PushItemWidth(50.0f);
			ImGui::SetCursorPos({ ImGui::GetCursorPosX(), 10 });
			if (ImGui::SliderFloat("Game Time: ", &scale, 0.0f, 2.0f, "%.2f"))
			{
				App->time->SetScaleGame(scale);
			}
			ImGui::SameLine();
			std::string gameCount = std::to_string(App->time->GetGameTimer());
			ImGui::Text(gameCount.data());
		}
	}
	ImGui::End();

}

void ModuleImGui::ChangePlayState(bool show, GameState state)
{
	assetsWindow = show;
	hierarchyWindow = show;
	App->time->gameState = state;

	std::string direction = "PlayingScene";
	if (show)
	{
		App->gameObject->LoadScene(direction.data());
		App->particle->StartAllEmiters();
	}
	else
		App->gameObject->SaveScene(direction.data());	
}

