/*
 * RenderUtils.cpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#include <math.h>

#include "RenderUtils.hpp"

vk2d::Vector2f vectorToVK2D(Vector2l& vec) {
	return {vec.x(), vec.y()};
}

float linearRGBtoSRGB(float color) {
	if (color <= 0.0031308) {
		return color * 12.92f;
	}
	return (1.055 * pow(color, 1 / 2.4) - 0.055);
}

float sRGBtoLinearRGB(float color){
	if (color <= 0.04045) {
		return color / 12.92f;
	}
	return pow((color + 0.055) / 1.055, 2.4);
}

void toLinearRGB(ImVec4* vec) {
	vec->x = sRGBtoLinearRGB(vec->x);
	vec->y = sRGBtoLinearRGB(vec->y);
	vec->z = sRGBtoLinearRGB(vec->z);
}

ImVec4 toLinearRGB(ImVec4 vec) {
	return { sRGBtoLinearRGB(vec.x), sRGBtoLinearRGB(vec.y), sRGBtoLinearRGB(vec.z), vec.w };
}
