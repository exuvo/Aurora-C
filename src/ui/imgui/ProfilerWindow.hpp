#ifndef SRC_UI_IMGUI_PROFILERWINDOW_HPP_
#define SRC_UI_IMGUI_PROFILERWINDOW_HPP_

#include "UIWindow.hpp"

class StarSystem;
class ProfilerEvents;

class ProfilerWindow : public UIWindow {
	public:
		ProfilerWindow(ImGuiLayer& layer): UIWindow(layer) {};
		virtual ~ProfilerWindow() = default;
		
		virtual void render() override;
		
	private:
		float zoom = 0.01f;
		int64_t renderScrollX = 0, oldRenderScrollX = 0;
		int64_t systemsScrollX = 0, oldSystemsScrollX = 0;
		StarSystem* oldStarSystem = nullptr;
};

#endif /* SRC_UI_IMGUI_PROFILERWINDOW_HPP_ */
