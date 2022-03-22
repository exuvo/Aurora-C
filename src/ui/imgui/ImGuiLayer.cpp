/*
 * ImGuiLayer.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: exuvo
 */

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include <VK2D.h>
#include <Interface/WindowImpl.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_vulkan.h>

#include "Aurora.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "ui/imgui/imgui_impl_glfw.h"
#include "ui/KeyMappings.hpp"
#include "ui/imgui/ImGuiDemoWindow.hpp"
#include "ui/imgui/MainDebugWindow.hpp"
#include "ui/imgui/EmpireOverviewWindow.hpp"
#include "ui/imgui/ShipDebugWindow.hpp"
#include "ui/imgui/ColonyManagerWindow.hpp"
#include "ui/imgui/ProfilerWindow.hpp"
#include "ui/imgui/ResearchWindow.hpp"
#include "ui/imgui/ShipDesignerWindow.hpp"
#include "ui/imgui/TopBarWindow.hpp"
#include "utils/dbg.h"
#include "utils/ImGuiUtils.hpp"
#include "utils/Utils.hpp"

static uint8_t imGuiLayers = 0;
static VkDescriptorPool vk_descriptorPool = VK_NULL_HANDLE;

ImGuiLayer::ImGuiLayer(AuroraWindow& parentWindow): UILayer(parentWindow) {
	IMGUI_CHECKVERSION();
	
	if (ImGui::GetCurrentContext() != nullptr) {
		imGuiGlfw = new ImGuiGlfw(window.window->impl->glfw_window, false);
		
		ImGui_ImplVulkan_InitViewport(imGuiGlfw->viewport);
  
	} else {
		ImGui::CreateContext();
		
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
		if (Aurora.settings.render.multiViewports) {
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		}
		io.ConfigViewportsNoTaskBarIcon = true;
		
		imGuiGlfw = new ImGuiGlfw(window.window->impl->glfw_window, true);
		
		initShared();
		
		ImGui::StyleColorsDark();
  
		ImGuiStyle& style = ImGui::GetStyle();
		style.ItemSpacing.y = 3;
		
		ImVec4* colors = style.Colors;
		colors[ImGuiCol_FrameBg]          = { 0.43, 0.43, 0.43, 0.39 };
		colors[ImGuiCol_FrameBgHovered]   = { 0.47, 0.47, 0.69, 0.40 };
		colors[ImGuiCol_FrameBgActive]    = { 0.42, 0.41, 0.64, 0.69 };
		colors[ImGuiCol_TitleBg]          = { 0.04, 0.04, 0.04, 0.87 };
		colors[ImGuiCol_TitleBgActive]    = { 0.32, 0.32, 0.63, 1.00 };
		colors[ImGuiCol_TitleBgCollapsed] = { 0.00, 0.00, 0.00, 0.51 };
		colors[ImGuiCol_Header]           = { 0.40, 0.40, 0.90, 0.45 };
		colors[ImGuiCol_HeaderHovered]    = { 0.45, 0.45, 0.90, 0.80 };
		colors[ImGuiCol_HeaderActive]     = { 0.53, 0.53, 0.87, 0.80 };
		colors[ImGuiCol_PlotLines]        = { 0.80, 0.80, 0.80, 1.00 };
		colors[ImGuiCol_PlotHistogram]    = { 0.90, 0.70, 0.00, 1.00 };
		
		// convert style from sRGB to linear https://github.com/ocornut/imgui/issues/578#issuecomment-577222389
		for (size_t i = 0; i < ImGuiCol_COUNT; i++) {
			toLinearRGB(&style.Colors[i]);
		}
	}
	
	layerID = imGuiLayers++;
	
	addWindow(new MainDebugWindow(*this));
	addWindow(new ImGuiDemoWindow(*this));
	addWindow(new EmpireOverviewWindow(*this));
	addWindow(new ShipDebugWindow(*this));
	addWindow(new ShipDesignerWindow(*this));
	addWindow(new ProfilerWindow(*this));
	addWindow(new ResearchWindow(*this));
	addWindow(new ColonyManagerWindow(*this));
	addWindow(new TopBarWindow(*this));
}

void ImGuiLayer::initShared(){
	bool initImGuiVulkan = false;
	bool initFontsTexture = false;
	
	{
		if (vk_descriptorPool == VK_NULL_HANDLE) {
			
			VkDescriptorPoolSize pool_sizes[] =
			{
					{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
					{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
					{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
					{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
					{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
					{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};
			
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
			pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
			pool_info.pPoolSizes = pool_sizes;
			
			auto result = vkCreateDescriptorPool(window.vk_device, &pool_info, nullptr, &vk_descriptorPool);
			check(result == VK_SUCCESS, "error creating descriptor pool: " << result);
		}
		
		ImGui_ImplVulkan_InitInfo init_info = { };
		init_info.Instance = window.vk_instance;
		init_info.PhysicalDevice = window.vk_physical_device;
		init_info.Device = window.vk_device;
		init_info.Queue = window.vk_render_queue.GetQueue(); // or secondary_render_queue
		init_info.QueueFamily = window.vk_render_queue.GetQueueFamilyIndex();
		init_info.PipelineCache = window.vk_pipeline_cache;
		init_info.DescriptorPool = vk_descriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2; // Seems to be unused
		init_info.ImageCount = window.window->impl->swapchain_image_count;
		init_info.CheckVkResultFn = [](VkResult result) {
			if (result != VK_SUCCESS) {
				Aurora.vk2dInstance->impl->Report(result, "Vulkan error in ImGui");
			}
		};
		
		initImGuiVulkan = ImGui_ImplVulkan_Init(&init_info, window.vk_render_pass);
		check(initImGuiVulkan, "failed to init ImGui vulkan");
		
		// Upload Fonts
		{
			// Use any command queue
			VkCommandPool command_pool = window.vk_command_pool;
			VkCommandBuffer command_buffer = window.vk_command_buffer;
			
			auto result = vkResetCommandPool(init_info.Device, command_pool, 0);
			check(result == VK_SUCCESS, "cannot reset command pool for fonts: " << result);
			
			VkCommandBufferBeginInfo begin_info = { };
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			
			result = vkBeginCommandBuffer(command_buffer, &begin_info);
			check(result == VK_SUCCESS, "failed to begin command pool for fonts: " << result);
			
			initFontsTexture = ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
			check(initFontsTexture, "failed to init ImGui vulkan fonts texture");
			
			VkSubmitInfo end_info = { };
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			
			result = vkEndCommandBuffer(command_buffer);
			check(result == VK_SUCCESS, "failed to end command pool for fonts: " << result);
			
			result = vkQueueSubmit(init_info.Queue, 1, &end_info, VK_NULL_HANDLE);
			check(result == VK_SUCCESS, "failed to submit queue for fonts: " << result);
			
			result = vkDeviceWaitIdle(init_info.Device);
			check(result == VK_SUCCESS, "failed to wait on device idle for fonts: " << result);
			
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
	}
	
	return;
	
	error:
	if (initFontsTexture) {
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
	
	if (initImGuiVulkan) {
		ImGui_ImplVulkan_Shutdown();
	}
	
	delete imGuiGlfw;
	ImGui::DestroyContext();
	
	throw std::runtime_error("Error creating ImGuiLayer");
}

ImGuiLayer::~ImGuiLayer() {
	if (imGuiGlfw != nullptr) {
		
		for (UIWindow* uiWindow : uiWindows) {
			delete uiWindow;
		}
		
		ImGui_ImplVulkan_ShutdownViewport(imGuiGlfw->viewport);
		
		if (--imGuiLayers == 0) {
			ImGui_ImplVulkan_Shutdown();
			
			if (vk_descriptorPool != VK_NULL_HANDLE) {
				vkDestroyDescriptorPool(window.vk_device, vk_descriptorPool, nullptr);
				vk_descriptorPool = VK_NULL_HANDLE;
			}
		}
		
//		if (imGuiGlfw->isMainWindow) {
//			ImGui::SetMainViewport(next window);
//		}
		
		delete imGuiGlfw;
		
		if (imGuiLayers == 0) {
			ImGui::DestroyContext();
		}
	}
}

void ImGuiLayer::preRender() {
	ZoneScopedN("ImGuiPre");
	ImGui_ImplVulkan_NewFrame();
	imGuiGlfw->NewFrame();
}

void ImGuiLayer::render() {
	// Record ImGui primitives into command buffer
	{
		ZoneScopedN("ImGui");
		
		ImGui::SetCurrentViewport(nullptr, (ImGuiViewportP*) imGuiGlfw->viewport);
		
//		ImGui::PushID(layerID); //TODO does not work as we have no window
		for (UIWindow* uiWindow : uiWindows) {
			if (uiWindow->visible) {
//				ImGui::PushID(uiWindow);
				uiWindow->render();
//				ImGui::PopID();
			}
		}
//		ImGui::PopID();
	}
}

void ImGuiLayer::postRender() {
	ZoneScopedN("ImGuiPost");
	
	ImDrawData* draw_data = ImGui::GetDrawData(imGuiGlfw->viewport);
	
	VkCommandBuffer command_buffer = window.vk_command_buffer;
	TracyVkZone(window.tracyVkCtxs[window.window->impl->next_image], command_buffer, "ImGui Render");
	ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
	
	window.restore_VK2D_render(true, true);
}

void ImGuiLayer::addWindow(UIWindow* uiWindow) {
	uiWindows.push_back(uiWindow);
}

void ImGuiLayer::removeWindow(UIWindow* uiWindow) {
	vectorEraseUnorderedVal(uiWindows, uiWindow);
	delete uiWindow;
}

bool ImGuiLayer::keyAction(KeyActions_ImGuiLayer action) {
	//TODO add all new permanent windows to main window imgui list
	// as they will close with this window otherwise even if moved elsewhere.
	
	if (action == KeyActions_ImGuiLayer::DEBUG) {
		
		toggleWindow<MainDebugWindow>();
		
	} else if (action == KeyActions_ImGuiLayer::SHIP_DEBUG) {
		
		toggleWindow<ShipDebugWindow>();
		
	} else if (action == KeyActions_ImGuiLayer::COLONY_MANAGER) {
		
		toggleWindow<ColonyManagerWindow>();
		
	} else if (action == KeyActions_ImGuiLayer::SHIP_DESIGNER) {
		
		toggleWindow<ShipDesignerWindow>();
		
	} else if (action == KeyActions_ImGuiLayer::RESEARCH) {
		
		toggleWindow<ResearchWindow>();
		
	} else if (action == KeyActions_ImGuiLayer::PROFILER) {
		
		toggleWindow<ProfilerWindow>();
	}
	
//	printf("PROFILER\n"); fflush(stdout);
	
	return false;
}

bool ImGuiLayer::eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	if (ImGui::GetCurrentContext()->IO.WantCaptureKeyboard) {
		imGuiGlfw->KeyCallback(nullptr, static_cast<int>(button), static_cast<int>(scancode), static_cast<int>(action), static_cast<int>(modifier_keys));
		return true;
	}
	
	KeyActions_ImGuiLayer keyBind = KeyMappings::getRaw<KeyActions_ImGuiLayer>(scancode, action, modifier_keys);
	
	if (keyBind != KeyActions_ImGuiLayer::NONE) {
		return keyAction(keyBind);
	}
	
	return false;
}

bool ImGuiLayer::eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
	if (ImGui::GetCurrentContext()->IO.WantCaptureKeyboard) {
		imGuiGlfw->CharCallback(nullptr, static_cast<unsigned int>(character));
		return true;
	}
	
	KeyActions_ImGuiLayer keyBind = KeyMappings::getTranslated<KeyActions_ImGuiLayer>(character);

	if (keyBind != KeyActions_ImGuiLayer::NONE) {
		return keyAction(keyBind);
	}
	
	return false;
}

bool ImGuiLayer::eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	if (ImGui::GetCurrentContext()->IO.WantCaptureMouse) {
		imGuiGlfw->MouseButtonCallback(nullptr, static_cast<int>(button), static_cast<int>(action), static_cast<int>(modifier_keys));
		return true;
	}
	
	return false;
}

bool ImGuiLayer::eventScroll(vk2d::Vector2d scroll) {
	if (ImGui::GetCurrentContext()->IO.WantCaptureMouse) {
		imGuiGlfw->ScrollCallback(nullptr, scroll.x, scroll.y);
		return true;
	}
	
	return false;
}
