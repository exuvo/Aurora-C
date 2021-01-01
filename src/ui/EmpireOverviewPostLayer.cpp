
/*
 * EmpireOverviewPostLayer.cpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#include "ui/imgui/ImGuiLayer.hpp"
#include "ui/imgui/EmpireOverviewWindow.hpp"
#include "EmpireOverviewPostLayer.hpp"

void EmpireOverviewPostLayer::render() {
	
	ImGuiLayer& imguiLayer = window.getLayer<ImGuiLayer>();
	EmpireOverviewWindow& empireOverviewWindow = imguiLayer.getWindow<EmpireOverviewWindow>();
	empireOverviewWindow.postDraw();	
}

