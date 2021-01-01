/*
 * EmpireOverviewWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_EMPIREOVERVIEWWINDOW_HPP_
#define SRC_UI_IMGUI_EMPIREOVERVIEWWINDOW_HPP_

#include <imgui_internal.h>

#include "UIWindow.hpp"
#include "utils/Math.hpp"

class EmpireOverviewPostLayer;
class EmpireOverviewWindow;
struct StrategicIconComponent;

struct DeferredIconDraw {
	StrategicIconComponent& icon;
	ImRect bb;
	float colorBits;
};

struct InlineIconDraw : DeferredIconDraw {
	EmpireOverviewWindow* window;
};

class EmpireOverviewWindow : public UIWindow {
	public:
		EmpireOverviewWindow(ImGuiLayer& layer): UIWindow(layer) {
			visible = true;
		};
		virtual ~EmpireOverviewWindow() = default;
		
		virtual void render() override;
		
	private:
		std::vector<InlineIconDraw> inlineIcons;
		std::vector<DeferredIconDraw> deferredIcons;
		friend EmpireOverviewPostLayer;
		
		void inlineDraw(DeferredIconDraw& deferredIcon);
		void postDraw();
};

#endif /* SRC_UI_IMGUI_EMPIREOVERVIEWWINDOW_HPP_ */
