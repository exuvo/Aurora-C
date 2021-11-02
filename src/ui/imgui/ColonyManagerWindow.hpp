/*
 * ResearchWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_COLONYMANAGERWINDOW_HPP_
#define SRC_UI_IMGUI_COLONYMANAGERWINDOW_HPP_

#include "UIWindow.hpp"

class ColonyManagerWindow : public UIWindow {
	public:
		ColonyManagerWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~ColonyManagerWindow() = default;
		
		virtual void render() override;
		
	private:
};

#endif /* SRC_UI_IMGUI_COLONYMANAGERWINDOW_HPP_ */
