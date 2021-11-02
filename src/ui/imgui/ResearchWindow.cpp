#include <imgui.h>
#include <imgui_internal.h>

#include "Aurora.hpp"
#include "ResearchWindow.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"

void ResearchWindow::render() {
	if (ImGui::Begin("Research", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar)) {

		
	}
	ImGui::End();
};
