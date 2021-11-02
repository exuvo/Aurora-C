/*
 * TopBarWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_TOPBARWINDOW_HPP_
#define SRC_UI_IMGUI_TOPBARWINDOW_HPP_

#include "UIWindow.hpp"

class TopBarWindow : public UIWindow {
	public:
		TopBarWindow(ImGuiLayer& layer): UIWindow(layer) { visible = true; };
		virtual ~TopBarWindow() = default;
		
		virtual void render() override;
		
	private:
};

#endif /* SRC_UI_IMGUI_TOPBARWINDOW_HPP_ */
