/*
 * UIWindow.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_UIWINDOW_HPP_
#define SRC_UI_UIWINDOW_HPP_

#include <imgui.h>
#include <imgui_sugar.hpp>

class ImGuiLayer;

class UIWindow {
	public:
		UIWindow(ImGuiLayer& layer): layer(layer) {};
		virtual ~UIWindow() = default;
		
		virtual void render() = 0;
		
		bool visible = false;
		
	protected:
		ImGuiLayer& layer;
};

#endif /* SRC_UI_UIWINDOW_HPP_ */
