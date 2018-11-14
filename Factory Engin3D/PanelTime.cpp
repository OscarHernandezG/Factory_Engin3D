#include "Application.h"
#include "ModuleImGui.h"

void ModuleImGui::CreateGameManager(float2 scale)
{
	ImGui::Begin("", &canScroll, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(playPos, playSize, scale, true);

	if (ImGui::Button("Play", { 50,25 }))
	{
		if (App->time->gameState == GameState_NONE)
		{
			App->time->gameState = GameState_PLAYING;
			App->gameObject->SaveBeforePlay();
		}
		else
		{
			App->time->gameState = GameState_STOP;
			App->gameObject->LoadAfterPlay();
		}
	}
	ImGui::SameLine();

	if (ImGui::Button("Pause", { 50,25 }))
	{
		if (App->time->gameState == GameState_PLAYING)
			App->time->gameState = GameState_PAUSE;
		else
			App->time->gameState = GameState_PLAYING;
	}
	ImGui::SameLine();

	if (ImGui::Button("Tick", { 50,25 }))
		App->time->gameState = GameState_TICK;


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

	ImGui::End();

}

