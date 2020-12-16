// dear imgui: Platform Binding for GLFW
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [x] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'. FIXME: 3 cursors types are missing from GLFW.
//  [X] Platform: Keyboard arrays indexed using GLFW_KEY_* codes, e.g. ImGui::IsKeyPressed(GLFW_KEY_SPACE).

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you are new to dear imgui, read examples/README.txt and read the documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once

#include "imgui.h"

struct GLFWwindow;
struct GLFWcursor;

class ImGuiGlfw {
	public:
		ImGuiGlfw(GLFWwindow* window);
		~ImGuiGlfw();
		void NewFrame();

		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void CharCallback(GLFWwindow* window, unsigned int c);

	private:
		GLFWwindow* g_Window;
		double g_Time = 0.0;
		bool g_MouseJustPressed[ImGuiMouseButton_COUNT] = { };
		GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT] = { };
		
		void UpdateMousePosAndButtons();
		void UpdateMouseCursor();
		void UpdateGamepads();
};

