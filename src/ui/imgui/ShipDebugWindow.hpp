/*
 * ShipDebugWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_SHIPDEBUGWINDOW_HPP_
#define SRC_UI_IMGUI_SHIPDEBUGWINDOW_HPP_

#include "UIWindow.hpp"

class ShipDebugWindow : public UIWindow {
	public:
		ShipDebugWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~ShipDebugWindow() = default;
		
		virtual void render() override;
		
	private:
};

#endif /* SRC_UI_IMGUI_SHIPDEBUGWINDOW_HPP_ */
