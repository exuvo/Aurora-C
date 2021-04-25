// dear imgui: Platform Binding for GLFW
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [x] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'. FIXME: 3 cursors types are missing from GLFW.
//  [X] Platform: Keyboard arrays indexed using GLFW_KEY_* codes, e.g. ImGui::IsKeyPressed(GLFW_KEY_SPACE).

// You can copy and use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// About GLSL version:
// The 'glsl_version' initialization parameter defaults to "#version 150" if NULL.
// Only override if your GL version doesn't handle this GLSL version. Keep NULL if unsure!

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

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

