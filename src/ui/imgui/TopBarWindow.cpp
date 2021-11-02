#include <imgui.h>
#include <imgui_internal.h>

#include "Aurora.hpp"
#include "TopBarWindow.hpp"
#include "galaxy/Player.hpp"
#include "galaxy/Empire.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"
#include "utils/ImGuiUtils.hpp"

void TopBarWindow::render() {
	
	Empire* empire = Player::current->empire;
	
	if (empire != nullptr) {
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 1,  0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 10,10 });
		
		ImVec2 viewportPos = ImGui::GetCurrentContext()->CurrentViewport->Pos;
		
		//TODO fix can get undocked when resizing viewport
		ImGui::SetNextWindowSize({ layer.getMainWindow().window->GetSize().x - 260, 16 });
		ImGui::SetNextWindowPos(viewportPos + ImVec2{ 260, 0 });
		ImGui::SetNextWindowBgAlpha(0.7f);
		
		if (ImGui::Begin("TopBar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {
			
			ImGui::Text("Empire: %s", empire->name.c_str());
			
		}
		ImGui::End();
		
		ImGui::PopStyleVar(2);
	}
}
