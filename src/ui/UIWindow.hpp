/*
 * UIWindow.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_UIWINDOW_HPP_
#define SRC_UI_UIWINDOW_HPP_

#include <imgui.h>

class UIWindow {
	public:
		UIWindow();
		virtual ~UIWindow();
		
		virtual void draw();
		
		bool visible;
};



#endif /* SRC_UI_UIWINDOW_HPP_ */
