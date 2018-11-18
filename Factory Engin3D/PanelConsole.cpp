#include "Application.h"
#include "ModuleImGui.h"
#include "imgui-1.65/imgui_dock.h"

void ModuleImGui::CreateConsole(float2 scale)
{
	ImGui::BeginDock("Console", &consoleWindow);

		//SetWindowDim(consolePos, consoleSize, scale);

		if (ImGui::Button("Clear", ImVec2(400, 20)))
			textBuff.clear();
	ImGui::Separator();

	ImGui::TextWrapped(textBuff.begin());
	if (canScroll)
		ImGui::SetScrollY(1.0f);
	canScroll = false;


	ImGui::EndDock();
}
void ModuleImGui::LogConsole(const char * consoleText)
{
	textBuff.appendf(consoleText);
	canScroll = true;
}
