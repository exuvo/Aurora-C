/*
 * MainDebugWindow.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_MAINDEBUGWINDOW_HPP_
#define SRC_UI_MAINDEBUGWINDOW_HPP_

#include "UIWindow.hpp"

class MainDebugWindow : public UIWindow {
	public:
		MainDebugWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~MainDebugWindow() = default;
		
		virtual void render() override;
		
	private:
		char stringbuf[10];
		float slider = 0;
};

#endif /* SRC_UI_MAINDEBUGWINDOW_HPP_ */
