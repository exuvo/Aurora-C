/*
 * Utils.hpp
 *
 *  Created on: 14 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_IMGUI_UTILS_HPP_
#define SRC_UTILS_IMGUI_UTILS_HPP_

#include <imgui.h>
#include <imgui_internal.h>

ImVec2 operator+(const ImVec2 a, const ImVec2 b);
ImVec2 operator+(const ImVec2 a, int b);
ImVec2 operator-(const ImVec2 a, int b);
ImRect operator+(const ImRect a, const ImRect b);
ImRect operator+(const ImRect a, int b);
ImRect operator-(const ImRect a, int b);

void toLinearRGB(ImVec4*);
__attribute__((warn_unused_result)) ImVec4 toLinearRGB(ImVec4);

void rightAlignedTableText(const char* text);

#endif /* SRC_UTILS_IMGUI_UTILS_HPP_ */
