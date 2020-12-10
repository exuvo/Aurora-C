/*
 * AuroraWindow.hpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_AURORAWINDOW_HPP_
#define SRC_UI_AURORAWINDOW_HPP_

#include <VK2D.h>

#include "log4cxx/logger.h"
#include "Tracy.hpp"

using namespace log4cxx;

class AuroraWindow {
	public:
		AuroraWindow();
		~AuroraWindow();
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.ui.window");
		std::unique_ptr<vk2d::Instance> vk2dInstance;
		vk2d::Window* window;
};

#endif /* SRC_UI_AURORAWINDOW_HPP_ */
