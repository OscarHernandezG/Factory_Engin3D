#include "Application.h"
#include "ModuleImGui.h"


void ModuleImGui::CreateConsole(float2 scale)
{
	ImGui::Begin("Console", &consoleWindow, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	SetWindowDim(consolePos, consoleSize, scale);

	if (ImGui::Button("Clear", ImVec2(400, 20)))
		textBuff.clear();
	ImGui::Separator();

	ImGui::TextWrapped(textBuff.begin());
	if (canScroll)
		ImGui::SetScrollY(1.0f);
	canScroll = false;

	ImGui::End();
}

void ModuleImGui::LogConsole(const char * consoleText)
{
	textBuff.appendf(consoleText);
	canScroll = true;
}
