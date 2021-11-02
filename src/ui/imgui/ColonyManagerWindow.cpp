#include <imgui.h>
#include <imgui_internal.h>

#include "Aurora.hpp"
#include "ColonyManagerWindow.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"

void ColonyManagerWindow::render() {
	if (ImGui::Begin("Colony Manager", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar)) {

		
	}
	ImGui::End();
};
