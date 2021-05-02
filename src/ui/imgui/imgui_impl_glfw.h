// dear imgui: Platform Binding for GLFW
// This needs to be used along with a Renderer (e.g. OpenGL3, Vulkan..)
// (Info: GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// Implemented features:
//  [X] Platform: Clipboard support.
//  [X] Platform: Gamepad support. Enable with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.
//  [x] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'. FIXME: 3 cursors types are missing from GLFW.
//  [X] Platform: Keyboard arrays indexed using GLFW_KEY_* codes, e.g. ImGui::IsKeyPressed(GLFW_KEY_SPACE).
//  [X] Platform: Multi-viewport support (multiple windows). Enable with 'io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable'.

// Issues:
//  [ ] Platform: Multi-viewport support: ParentViewportID not honored, and so io.ConfigViewportsNoDefaultParent has no effect (minor).

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
struct GLFWmonitor;

class ImGuiGlfw {
	public:
		ImGuiGlfw(GLFWwindow* window, bool isMainWindow);
		~ImGuiGlfw();
		void NewFrame();

		bool isMainWindow;
		ImGuiViewport* viewport = nullptr;
		
		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void CharCallback(GLFWwindow* window, unsigned int c);
		
		static void MouseButtonPlatformCallback(GLFWwindow* window, int button, int action, int mods);
		static void ScrollPlatformCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void KeyPlatformCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void CharPlatformCallback(GLFWwindow* window, unsigned int c);

	private:
		GLFWwindow* g_Window;
		static double g_Time;// = 0.0;
		static bool g_MouseJustPressed[ImGuiMouseButton_COUNT];// = { };
		static GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT];// = { };
		static GLFWwindow* g_KeyOwnerWindows[512];// = {};
		static bool g_WantUpdateMonitors;// = true;
		
		void UpdateMousePosAndButtons();
		void UpdateMouseCursor();
		void UpdateGamepads();
		void UpdateMonitors();
		
		static const char* GetClipboardText(void* user_data);
		static void SetClipboardText(void* user_data, const char* text);
		
		// Multi-viewport
		void InitPlatformInterface();
		void ShutdownPlatformInterface();
		static int CreateVkSurface(ImGuiViewport* viewport, ImU64 vk_instance, const void* vk_allocator, ImU64* out_vk_surface);
		static void CreateWindow(ImGuiViewport* viewport);
		static void DestroyWindow(ImGuiViewport* viewport);
		static void ShowWindow(ImGuiViewport* viewport);
		static ImVec2 GetWindowPos(ImGuiViewport* viewport);
		static void SetWindowPos(ImGuiViewport* viewport, ImVec2 pos);
		static ImVec2 GetWindowSize(ImGuiViewport* viewport);
		static void SetWindowSize(ImGuiViewport* viewport, ImVec2 size);
		static void SetWindowTitle(ImGuiViewport* viewport, const char* title);
		static void SetWindowFocus(ImGuiViewport* viewport);
		static bool GetWindowFocus(ImGuiViewport* viewport);
		static bool GetWindowMinimized(ImGuiViewport* viewport);
		static void SetWindowAlpha(ImGuiViewport* viewport, float alpha);
		static void RenderWindow(ImGuiViewport* viewport, void*);
		static void SwapBuffers(ImGuiViewport* viewport, void*);
		static void SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos); // Win32 only
		
		static void MonitorCallback(GLFWmonitor* monitor, int event);
		static void WindowCloseCallback(GLFWwindow* window);
		static void WindowPosCallback(GLFWwindow* window, int, int);
		static void WindowSizeCallback(GLFWwindow* window, int, int);
};
