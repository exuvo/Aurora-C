/*
 * ImGuiDemoWindow.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: exuvo
 */

#include <imgui.h>
#include <imgui_internal.h>

#include "Aurora.hpp"
#include "MainDebugWindow.hpp"
#include "ui/imgui/ImGuiDemoWindow.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"

void MainDebugWindow::render() {
	if (ImGui::Begin("Debug window", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar)) {

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Windows")) {
//				ImGui::MenuItem("Ship debug", "", shipDebugger::visible);
//				ImGui::MenuItem("Ship designer", "", shipDesigner::visible);
//				ImGui::MenuItem("Colony manager", "", colonyManager::visible);
				ImGui::MenuItem("ImGui Demo", "hotkey", &(layer.getWindow<ImGuiDemoWindow>().visible), true);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Render")) {
//				if (ImGui::MenuItem("Show PassiveSensor hits", "", Aurora.settings.render.debugPassiveSensors)) {
//					Aurora.settings.render..debugPassiveSensors = !Aurora.settings.render..debugPassiveSensors;
//				}
//				if (ImGui::MenuItem("DisableStrategicView", "", Aurora.settings.render..debugDisableStrategicView)) {
//					Aurora.settings.render..debugDisableStrategicView = !Aurora.settings.render..debugDisableStrategicView;
//				}
//				if (ImGui::MenuItem("DrawWeaponRangesWithoutShader", "", Aurora.settings.render..debugDrawWeaponRangesWithoutShader)) {
//					Aurora.settings.render..debugDrawWeaponRangesWithoutShader = !Aurora.settings.render..debugDrawWeaponRangesWithoutShader;
//				}
				ImGui::MenuItem("useShadow", "", &Aurora.settings.render.useShadow, true);
				ImGui::MenuItem("debugSpatialPartitioning", "", &Aurora.settings.render.debugSpatialPartitioning, true);
				ImGui::MenuItem("debugSpatialPartitioningPlanetoids", "", &Aurora.settings.render.debugSpatialPartitioningPlanetoids, true);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		auto ctx = ImGui::GetCurrentContext();
		
		ImGui::Text("Hello, world %d", 4);
		ImGui::Text("ctx.hoveredWindow %s", ctx->HoveredWindow != nullptr ? ctx->HoveredWindow->Name : "null");
		ImGui::Text("ctx.navWindow %s", ctx->NavWindow != nullptr ? ctx->NavWindow->Name : "null");
//		ImGui::Text("ctx.navWindow.dc %ul" , ctx->NavWindow != nullptr ? ctx->NavWindow->DC : 0);
		ImGui::Text("ctx.io.wantCaptureMouse %u", ctx->IO.WantCaptureMouse);
		ImGui::Text("ctx.io.wantCaptureKeyboard %u", ctx->IO.WantCaptureKeyboard);
		float graphValues[] = { 0, 5, 2, 4 };
		ImGui::PlotLines("plot", graphValues, ARRAY_LEN(graphValues), 0, nullptr, 0, 5, {}, 1);

		if (ImGui::Button("OK")) {
			std::cout << "click" << std::endl;
		}

		ImGui::InputText("string", stringbuf, ARRAY_LEN(stringbuf), ImGuiInputTextFlags_::ImGuiInputTextFlags_None, nullptr, nullptr);
		ImGui::SliderFloat("float", &slider, 0, 1);
//		ImGui::Image(img.getTexture().textureObjectHandle, Vec2(64, 64));
	}
	ImGui::End();
};
