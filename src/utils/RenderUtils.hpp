/*
 * RenderUtils.hpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_RENDERUTILS_HPP_
#define SRC_UTILS_RENDERUTILS_HPP_

#include <VK2D.h>
#include <imgui.h>

#include "utils/Math.hpp"

__attribute__((warn_unused_result)) vk2d::Vector2f vectorToVK2D(Vector2l vec);
__attribute__((warn_unused_result)) vk2d::Vector2f vectorToVK2D(Vector2i vec);
__attribute__((warn_unused_result)) Vector2i vk2dToVector(vk2d::Vector2i vec);
__attribute__((warn_unused_result)) Vector2d vk2dToVector(vk2d::Vector2d vec);
__attribute__((warn_unused_result)) vk2d::Rect2f matrixToVK2D(Matrix2i mat);

float linearRGBtoSRGB(float);
float sRGBtoLinearRGB(float);
void toLinearRGB(ImVec4*);
__attribute__((warn_unused_result)) ImVec4 toLinearRGB(ImVec4);

#endif /* SRC_UTILS_RENDERUTILS_HPP_ */