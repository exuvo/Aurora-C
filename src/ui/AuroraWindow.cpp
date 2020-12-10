/*
 * AuroraWindow.cpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#include <iostream>

#include "AuroraWindow.hpp"

void VK2D_APIENTRY VK2D_log(vk2d::ReportSeverity severity, std::string_view message) {
	std::cout << message << std::endl;
}

AuroraWindow::AuroraWindow() {
	
	std::cout <<  "creating vk2d instance" << std::endl;
	
	vk2d::InstanceCreateInfo instance_create_info {};
	instance_create_info.application_name = "Aurora C";
	instance_create_info.application_version = "?";
	instance_create_info.engine_name = "VK2D";
	instance_create_info.engine_version = "?";
	instance_create_info.report_function = VK2D_log;
	
	vk2dInstance = vk2d::CreateInstance(instance_create_info);
	
	if (!vk2dInstance) {
		std::cout << "failed to create vk2d instance" << std::endl;
		
	} else {
		
		std::cout << "creating window" << std::endl;
		
		vk2d::WindowCreateInfo window_create_info{};
		window_create_info.size = { 512, 512 };
		window_create_info.coordinate_space = vk2d::RenderCoordinateSpace::TEXEL_SPACE_CENTERED;
		window = vk2dInstance->CreateOutputWindow(window_create_info);
		
		if (!window)  {
			std::cout << "failed to create window" << std::endl;
		}
	}
}

AuroraWindow::~AuroraWindow() {
	if (window) {
		vk2dInstance->DestroyOutputWindow(window);
		window = nullptr;
	}
}
