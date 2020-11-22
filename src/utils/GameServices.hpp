/*
 * GameServices.hpp
 *
 *  Created on: 22 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_GAMESERVICES_HPP_
#define SRC_UTILS_GAMESERVICES_HPP_

#include "entt/entt.hpp"

#include "galaxy/Galaxy.hpp"

struct GameServices {
	using galaxy = entt::service_locator<Galaxy>;
};

#endif /* SRC_UTILS_GAMESERVICES_HPP_ */
