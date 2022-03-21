/*
 * ImGuiUtils.cpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#include "ImGuiUtils.hpp"
#include "utils/RenderUtils.hpp"

ImVec2 operator+(const ImVec2 a, const ImVec2 b) {
	return {a.x + b.x, a.y + b.y};
}

ImVec2 operator+(const ImVec2 a, int b) {
	return {a.x + b, a.y + b};
}

ImVec2 operator-(const ImVec2 a, int b) {
	return {a.x - b, a.y - b};
}

ImRect operator+(const ImRect a, const ImRect b) {
	return {a.Min + b.Min, a.Max + b.Max};
}

ImRect operator+(const ImRect a, int b) {
	return {a.Min + b, a.Max + b};
}

ImRect operator-(const ImRect a, int b) {
	return {a.Min - b, a.Max - b};
}

void toLinearRGB(ImVec4* vec) {
	vec->x = sRGBtoLinearRGB(vec->x);
	vec->y = sRGBtoLinearRGB(vec->y);
	vec->z = sRGBtoLinearRGB(vec->z);
}

ImVec4 toLinearRGB(ImVec4 vec) {
	return { sRGBtoLinearRGB(vec.x), sRGBtoLinearRGB(vec.y), sRGBtoLinearRGB(vec.z), vec.w };
}

void rightAlignedText(int maxWidth, const char* text, ...) {
	va_list args;
	va_start(args, text);
	char buf[128];
	const char* bufEnd = buf + ImFormatStringV(buf, IM_ARRAYSIZE(buf), text, args);
	va_end(args);
	
	ImGui::GetCurrentWindow()->DC.CursorPos.x = std::max(ImGui::GetCurrentWindow()->DC.CursorPos.x, 
	                                            ImGui::GetCurrentWindow()->DC.CursorPos.x
	                                          + maxWidth
	                                          - ImGui::CalcTextSize(buf, bufEnd).x);
	ImGui::TextEx(buf, bufEnd);
}

void rightAlignedTableText(const char* text, ...) {
	va_list args;
	va_start(args, text);
	char buf[128];
	const char* bufEnd = buf + ImFormatStringV(buf, IM_ARRAYSIZE(buf), text, args);
	va_end(args);
	
	ImGui::GetCurrentWindow()->DC.CursorPos.x = std::max(ImGui::GetCurrentWindow()->DC.CursorPos.x,
	                                            ImGui::GetCurrentWindow()->DC.CursorPos.x
	                                          + ImGui::GetCurrentTable()->Columns[ImGui::TableGetColumnIndex()].WidthGiven
	                                          - ImGui::CalcTextSize(buf, bufEnd).x);
	ImGui::TextEx(buf, bufEnd);
}
