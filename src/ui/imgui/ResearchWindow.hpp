/*
 * ResearchWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_RESEARCHWINDOW_HPP_
#define SRC_UI_IMGUI_RESEARCHWINDOW_HPP_

#include "UIWindow.hpp"

class ResearchWindow : public UIWindow {
	public:
		ResearchWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~ResearchWindow() = default;
		
		virtual void render() override;
		
	private:
};

#endif /* SRC_UI_IMGUI_RESEARCHWINDOW_HPP_ */
