/*
 * AuroraWindow.hpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_AURORAWINDOW_HPP_
#define SRC_UI_AURORAWINDOW_HPP_

#include <vulkan/vulkan_core.h>
#include <VK2D.h>
#include <System/QueueResolver.h>
#include <log4cxx/logger.h>

#include "utils/Utils.hpp"
#include "utils/Profiling.hpp"
#include "utils/Math.hpp"

using namespace log4cxx;

namespace tracy {
	class VkCtx;
}

using TracyVkCtx = tracy::VkCtx*;

class UILayer;
class StarSystem;

template<typename T>
concept aUILayer = std::is_base_of<UILayer, T>::value;

class AuroraWindow : vk2d::WindowEventHandler {
	public:
		AuroraWindow(StarSystem* starSystem);
		virtual ~AuroraWindow();
		
		void render();
		void addLayer(UILayer* layer);
		void setMainLayer(UILayer* layer);
		bool isKeyPressed(int glfwKey);
		void restore_VK2D_render(bool viewportAndScissor, bool windowFrameData);
		
		vk2d::Window* window;
		std::vector<TracyVkCtx> tracyVkCtxs;
		std::vector<UILayer*> layers;
		vk2d::Vector2i mousePos;
		
		float zoom = 5E5; // 1E6
		Vector2l viewOffset {0,0}; // in m
		
		StarSystem* starSystem;
		ProfilerEvents profilerEvents;
		
		VkInstance vk_instance;
		VkDevice vk_device;
		VkPhysicalDevice vk_physical_device;
		VkRenderPass vk_render_pass;
		VkExtent2D vk_extent;
		VkCommandBuffer vk_command_buffer;
		vk2d::_internal::ResolvedQueue vk_render_queue;
		vk2d::_internal::ResolvedQueue vk_transfer_queue;
		VkPipelineCache vk_pipeline_cache;
		VkCommandPool vk_command_pool;
		
		template<aUILayer T>
		T& getLayer() {
			for (UILayer* layer : layers) {
				auto casted = dynamic_cast<T*>(layer);
				if (casted) {
					return *casted;
				}
			}
			
			throw std::invalid_argument("No ui layer of type " + type_name<T>());
		}
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.ui.window");
		nanoseconds lastDrawStart = getNanos();
		nanoseconds frameTime = 0s;
		nanoseconds renderTime = 0s;
		double frameTimeAverage = 0;
		double renderTimeAverage = 0;
		
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
