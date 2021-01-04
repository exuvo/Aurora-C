/*
 * AuroraWindow.cpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#include <iostream>
#include <fmt/core.h>

#include <Tracy.hpp>
#include <TracyVulkan.hpp>

#include <VK2D.h>
#include <Interface/InstanceImpl.h>
#include <Interface/WindowImpl.h>

#include "Aurora.hpp"
#include "ui/AuroraWindow.hpp"
#include "ui/UILayer.hpp"
#include "ui/StarSystemLayer.hpp"
#include "ui/ShaderTestLayer.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "ui/RenderCache.hpp"
#include "utils/dbg.h"
#include "utils/Utils.hpp"
#include "utils/Math.hpp"

using namespace std::chrono;

AuroraWindow::AuroraWindow() {
	
	std::cout << "creating window" << std::endl;
	
	vk2d::WindowCreateInfo window_create_info{};
	window_create_info.title = "Aurora C";
	window_create_info.size = { 1024, 768 };
	window_create_info.coordinate_space = vk2d::RenderCoordinateSpace::TEXEL_SPACE;
	window_create_info.event_handler = this;
//		window_create_info.fullscreen_monitor = ?
//		window_create_info.fullscreen_refresh_rate = ?
	window_create_info.vsync = Aurora.settings.render.vsync;
	window = Aurora.vk2dInstance->CreateOutputWindow(window_create_info);
	check(window, "failed to create window");
	
	for (size_t i = 0; i < window->impl->vk_render_command_buffers.size(); i++) {
		TracyVkCtx tmp = TracyVkContext(Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice(),
		                                Aurora.vk2dInstance->impl->GetVulkanDevice(),
		                                Aurora.vk2dInstance->impl->GetPrimaryRenderQueue().GetQueue(),
		                                window->impl->vk_render_command_buffers[i]);
		tracyVkCtxs.push_back(tmp);
	}
	
	return;
	
	error:
		if (window) {
			for (TracyVkCtx tracyVkCtx : tracyVkCtxs) {
				TracyVkDestroy(tracyVkCtx);
			}
			
			for (UILayer* layer : layers) {
				delete layer;
			}
			
			Aurora.vk2dInstance->DestroyOutputWindow(window);
		}
		
		throw std::runtime_error("Error creating AuroraWindow");
}

AuroraWindow::~AuroraWindow() {
	if (window) {
		
		for (TracyVkCtx tracyVkCtx : tracyVkCtxs) {
			TracyVkDestroy(tracyVkCtx);
		}
		
		for (UILayer* layer : layers) {
			delete layer;
		}
		
		Aurora.vk2dInstance->DestroyOutputWindow(window);
		window = nullptr;
	}
	
	vectorEraseUnorderedVal(Aurora.windows, this);
	
	if (Aurora.windows.size() == 0) {
		Aurora.shutdown = true;
	}
}

void AuroraWindow::render() {
	nanoseconds now = getNanos();
	frameTime = now - lastDrawStart;
	lastDrawStart = now;
	
	frameTimeAverage = exponentialAverage(frameTime.count(), frameTimeAverage, 15.0);
	
	ZoneScoped;
	
	if(!window->BeginRender()) {
		LOG4CXX_ERROR(log, "Error rendering window begin");
		return;
	}
	
	{
		ZoneScopedN("Draw");
		TracyVkZone(tracyVkCtxs[window->impl->next_image], window->impl->vk_render_command_buffers[window->impl->next_image], "Render");
		
//		float x = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count() % (100 + window->GetSize().x) - window->GetSize().x / 2 - 100;
//		window->DrawRectangle(
//			{{x, 0}, {x + 100, 100}},
//			true,
//			vk2d::Colorf::RED()
//		);
		
		for (UILayer* layer : layers) {
			try {
				layer->render();
			} catch (const std::exception& e) {
				std::string stackTrace = getLastExceptionStacktrace();
				LOG4CXX_ERROR(log, "Exception in rendering layer " << demangleTypeName(typeid(*layer).name()) << ": " << e.what() << "\n" << stackTrace);
			}
		}
		
		uint32_t centinanosFrameStartTime = (frameTime.count() / 10000) % 100;
		uint32_t milliFrameStartTime = frameTime.count() / Units::NANO_MILLI;
		
		uint32_t centinanosFrameTimeAverage = ((uint64_t) frameTimeAverage / 10000) % 100;
		uint32_t milliFrameTimeAverage = (uint64_t) frameTimeAverage / Units::NANO_MILLI;
		
		uint32_t centinanosRenderTimeAverage = ((uint64_t) renderTimeAverage / 10000) % 100;
		uint32_t milliRenderTimeAverage = (uint64_t) renderTimeAverage / Units::NANO_MILLI;
		
		std::string text = fmt::format("{} {:02}.{:02}ms {:02}.{:02}ms, {:02}.{:02}ms", Aurora.fps, milliFrameStartTime, centinanosFrameStartTime, milliFrameTimeAverage, centinanosFrameTimeAverage, milliRenderTimeAverage, centinanosRenderTimeAverage);
		vk2d::Mesh text_mesh = vk2d::GenerateTextMesh(Aurora.assets.font, { 2, 15 }, text);
		window->DrawMesh(text_mesh);
	}
	
	if(!window->EndRender()) {
		LOG4CXX_ERROR(log, "Error rendering window end");
		return;
	}

	renderTime = getNanos() - now;
	renderTimeAverage = exponentialAverage(renderTime.count(), renderTimeAverage, 10.0);
	
	FrameMark
}

void AuroraWindow::addLayer(UILayer* layer) {
	layers.push_back(layer);
}

void AuroraWindow::setMainLayer(UILayer* layer) {
	if (layers.size() == 0) {
		layers.push_back(layer);
	} else {
		delete layers[0];
		layers[0] = layer;
	}
}

bool AuroraWindow::isKeyPressed(int glfwKey) {
	return glfwGetKey(window->impl->glfw_window, glfwKey) == GLFW_PRESS;
}

void AuroraWindow::EventMouseButton(vk2d::Window*	window, vk2d::MouseButton	button, vk2d::ButtonAction action,
		                                vk2d::ModifierKeyFlags modifier_keys) {
	for (auto it = layers.end(); it-- != layers.begin();) {
		UILayer* layer = *it;
		if (layer->eventMouseButton(button, action, modifier_keys)) return;
	}
}

void AuroraWindow::EventWindowPosition(vk2d::Window* window, vk2d::Vector2i position) {
}

void AuroraWindow::EventScreenshot(vk2d::Window* window, const std::filesystem::__cxx11::path& screenshot_path,
                                   const vk2d::ImageData& screenshot_data, bool success,
                                   const std::string& error_message) {
}

void AuroraWindow::EventCursorEnter(vk2d::Window* window, bool entered) {
//	std::cout << "window cursorEnter " << entered << std::endl;
}

void AuroraWindow::EventWindowFocus(vk2d::Window* window, bool focused) {
//	std::cout << "window focus " << focused << std::endl;
}

void AuroraWindow::EventCursorPosition(vk2d::Window* window, vk2d::Vector2d position) {
	mousePos = {position.x, position.y};
}

void AuroraWindow::EventFileDrop(vk2d::Window* window,
    std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> > files) {
}

void AuroraWindow::EventScroll(vk2d::Window* window, vk2d::Vector2d scroll) {
//	std::cout << "window scroll" << std::endl;
	for (auto it = layers.end(); it-- != layers.begin();) {
		UILayer* layer = *it;
		if (layer->eventScroll(scroll)) return;
	}
}

void AuroraWindow::EventWindowIconify(vk2d::Window* window, bool iconified) {
	std::cout << "window iconify " << iconified << std::endl;
}

void AuroraWindow::EventWindowClose(vk2d::Window* window) {
//	std::cout << "window close" << std::endl;
	window->CloseWindow();
}

void AuroraWindow::EventCharacter(vk2d::Window* window, uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
	if (character == 'n') {
		AuroraWindow* window = new AuroraWindow();
		window->setMainLayer(new StarSystemLayer(*window, getLayer<StarSystemLayer>().getStarSystem()));
		window->addLayer(new ImGuiLayer(*window));
		window->addLayer(new ShaderTestLayer(*window));
		Aurora.windows.push_back(window);
		return;
	}
	
	for (auto it = layers.end(); it-- != layers.begin();) {
		UILayer* layer = *it;
		if (layer->eventCharacter(character, modifier_keys)) return;
	}
}

void AuroraWindow::EventWindowSize(vk2d::Window* window, vk2d::Vector2u size) {
//	std::cout << "window resize" << std::endl;
}

void AuroraWindow::EventWindowMaximize(vk2d::Window* window, bool maximized) {
//	std::cout << "window maximized " << maximized << std::endl;
}

void AuroraWindow::EventKeyboard(vk2d::Window* window, vk2d::KeyboardButton button, int32_t scancode,
                                 vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	for (auto it = layers.end(); it-- != layers.begin();) {
		UILayer* layer = *it;
		if (layer->eventKeyboard(button, scancode, action, modifier_keys)) return;
	}
}
