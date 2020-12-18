/*
 * RenderUtils.cpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#include "RenderUtils.hpp"

vk2d::Vector2f vectorToVK2D(Vector2l& vec) {
	return {vec.x(), vec.y()};
}
