/*
 * AuroraWindow.hpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_AURORAWINDOW_HPP_
#define SRC_UI_AURORAWINDOW_HPP_

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include <VK2D.h>
#include <examples/imgui_impl_vulkan.h>

#include "log4cxx/logger.h"

using namespace log4cxx;

class AuroraWindow : vk2d::WindowEventHandler {
	public:
		AuroraWindow();
		virtual ~AuroraWindow();
		
		void render();

		vk2d::Window* window;
		ImGuiContext* imGuiCtx = nullptr;
		std::vector<TracyVkCtx> tracyVkCtxs;
//		std::unique_ptr<vk2d::Instance> vk2dInstance;
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.ui.window");
		
		void EventMouseButton(vk2d::Window*	window, vk2d::MouseButton	button, vk2d::ButtonAction action,
		                                    vk2d::ModifierKeyFlags modifier_keys) override;
		void EventWindowPosition(vk2d::Window* window, vk2d::Vector2i position) override;
		void EventScreenshot(vk2d::Window* window, const std::filesystem::__cxx11::path& screenshot_path,
		                     const vk2d::ImageData& screenshot_data, bool success, const std::string& error_message)
		                         override;
		void EventCursorEnter(vk2d::Window* window, bool entered) override;
		void EventWindowFocus(vk2d::Window* window, bool focused) override;
		void EventCursorPosition(vk2d::Window* window, vk2d::Vector2d position) override;
		void EventFileDrop(vk2d::Window* window,
		                   std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> > files) override;
		void EventScroll(vk2d::Window* window, vk2d::Vector2d scroll) override;
		void EventWindowIconify(vk2d::Window* window, bool iconified) override;
		void EventWindowClose(vk2d::Window* window) override;
		void EventCharacter(vk2d::Window* window, uint32_t character, vk2d::ModifierKeyFlags modifier_keys) override;
		void EventWindowSize(vk2d::Window* window, vk2d::Vector2u size) override;
		void EventWindowMaximize(vk2d::Window* window, bool maximized) override;
		void EventKeyboard(vk2d::Window* window, vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action,
		                   vk2d::ModifierKeyFlags modifier_keys) override;
};

#endif /* SRC_UI_AURORAWINDOW_HPP_ */
