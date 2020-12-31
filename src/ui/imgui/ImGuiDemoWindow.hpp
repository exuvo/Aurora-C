/*
 * ImGuiDemoWindow.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUIDEMOWINDOW_HPP_
#define SRC_UI_IMGUIDEMOWINDOW_HPP_

#include "UIWindow.hpp"

class ImGuiDemoWindow : public UIWindow {
	public:
		ImGuiDemoWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~ImGuiDemoWindow() override = default;
		
		virtual void render() override {
			ImGui::ShowDemoWindow(&visible);
		};
};

#endif /* SRC_UI_IMGUIDEMOWINDOW_HPP_ */
