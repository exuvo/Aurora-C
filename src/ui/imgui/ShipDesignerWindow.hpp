/*
 * ResearchWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_SHIPDESIGNERWINDOW_HPP_
#define SRC_UI_IMGUI_SHIPDESIGNERWINDOW_HPP_

#include "UIWindow.hpp"

class ShipDesignerWindow : public UIWindow {
	public:
		ShipDesignerWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~ShipDesignerWindow() = default;
		
		virtual void render() override;
		
	private:
};

#endif /* SRC_UI_IMGUI_SHIPDESIGNERWINDOW_HPP_ */
