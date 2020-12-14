/*
 * Aurora.hpp
 *
 *  Created on: Dec 14, 2020
 *      Author: exuvo
 */

#ifndef SRC_AURORA_HPP_
#define SRC_AURORA_HPP_

#include <vector>
#include <VK2D.h>

#include "ui/AuroraWindow.hpp"

class Galaxy;

struct AuroraGlobal {
//	using galaxy = entt::service_locator<Galaxy>;
		Galaxy* galaxy;
		std::vector<AuroraWindow*> windows;
		bool shutdown = false;
		vk2d::Instance* vk2dInstance;
};

extern AuroraGlobal Aurora;

#endif /* SRC_AURORA_HPP_ */
