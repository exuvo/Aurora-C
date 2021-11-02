#include <imgui.h>
#include <imgui_internal.h>

#include "Aurora.hpp"
#include "ShipDesignerWindow.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"

void ShipDesignerWindow::render() {
	if (ImGui::Begin("Ship designer", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar)) {

		
	}
	ImGui::End();
};
