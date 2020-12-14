/*
 * AuroraWindow.cpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#include <iostream>

#include "AuroraWindow.hpp"
#include "Aurora.hpp"
#include "utils/Utils.hpp"

AuroraWindow::AuroraWindow() {
	
	std::cout << "creating window" << std::endl;
	
	vk2d::WindowCreateInfo window_create_info{};
	window_create_info.size = { 512, 512 };
	window_create_info.coordinate_space = vk2d::RenderCoordinateSpace::TEXEL_SPACE_CENTERED;
	window_create_info.event_handler = this;
//		window_create_info.fullscreen_monitor = ?
//		window_create_info.fullscreen_refresh_rate = ?
//		window_create_info.vsync = ?
	window = Aurora.vk2dInstance->CreateOutputWindow(window_create_info);
	
	if (!window)  {
		LOG4CXX_ERROR(log, "failed to create window");
		throw std::runtime_error("failed to create window");
	}
}

void AuroraWindow::render() {
	if(!window->BeginRender()) {
		LOG4CXX_ERROR(log, "Error rendering window begin");
		return;
	}
	
	window->DrawRectangle(
		{{0, 0}, {100,100}},
		true,
		vk2d::Colorf( 1.0f, 0.0f, 0.0f, 1.0f )
	);
	
	if(!window->EndRender()) {
		LOG4CXX_ERROR(log, "Error rendering window end");
		return;
	}
}

AuroraWindow::~AuroraWindow() {
	if (window) {
		Aurora.vk2dInstance->DestroyOutputWindow(window);
		window = nullptr;
	}
	
	vectorEraseUnorderedVal(Aurora.windows, this);
	
	if (Aurora.windows.size() == 0) {
		Aurora.shutdown = true;
	}
}

void AuroraWindow::EventMouseButton(vk2d::Window*	window, vk2d::MouseButton	button, vk2d::ButtonAction action,
		                                vk2d::ModifierKeyFlags modifier_keys) {
	
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
	std::cout << "window focus " << focused << std::endl;
}

void AuroraWindow::EventCursorPosition(vk2d::Window* window, vk2d::Vector2d position) {
}

void AuroraWindow::EventFileDrop(vk2d::Window* window,
    std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> > files) {
}

void AuroraWindow::EventScroll(vk2d::Window* window, vk2d::Vector2d scroll) {
	std::cout << "window scroll" << std::endl;
}

void AuroraWindow::EventWindowIconify(vk2d::Window* window, bool iconified) {
	std::cout << "window iconify " << iconified << std::endl;
}

void AuroraWindow::EventWindowClose(vk2d::Window* window) {
	std::cout << "window close" << std::endl;
	window->CloseWindow();
}

void AuroraWindow::EventCharacter(vk2d::Window* window, uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
}

void AuroraWindow::EventWindowSize(vk2d::Window* window, vk2d::Vector2u size) {
	std::cout << "window resize" << std::endl;
}

void AuroraWindow::EventWindowMaximize(vk2d::Window* window, bool maximized) {
	std::cout << "window maximized " << maximized << std::endl;
}

void AuroraWindow::EventKeyboard(vk2d::Window* window, vk2d::KeyboardButton button, int32_t scancode,
                                 vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
}
