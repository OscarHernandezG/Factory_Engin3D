#include "Application.h"
#include "ModuleImGui.h"
#include "ComponentEmitter.h"

void ModuleImGui::CreateGameTime()
{
	if (ImGui::Begin("GameSimulate", &gameTimeWindow, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize))
	{
		ComponentEmitter* emitter = (ComponentEmitter*)App->geometry->currentGameObject->GetComponent(ComponentType_EMITTER);

		if (ImGui::Button("Play", { 50,25 }))
		{
			if(emitter->simulatedGame == GameState_PAUSE)
				emitter->timeSimulating.Continue();
			else
				emitter->timeSimulating.Start();

			emitter->simulatedGame = GameState_PLAYING;
			emitter->StartEmitter();			
		}

		ImGui::SameLine();

		if (ImGui::Button("Pause", { 50,25 }))
		{
			emitter->simulatedGame = GameState_PAUSE;
			emitter->timeSimulating.Stop();
		}
		ImGui::SameLine();

		if (ImGui::Button("Stop", { 50,25 }))
		{
			emitter->simulatedGame = GameState_STOP;
			emitter->emitterActive = false;
		}

		ImGui::Text("Game timer: ");

		ImGui::SameLine();
		std::string gameCount = std::to_string(emitter->timeSimulating.ReadSec());
		ImGui::Text(gameCount.data());

	}
	
	ImGui::End();

}
