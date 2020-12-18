/*
 * ImGuiLayer.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: exuvo
 */

#include "Tracy.hpp"
#include "TracyVulkan.hpp"

#include <VK2D.h>
#include <Interface/InstanceImpl.h>
#include <Interface/WindowImpl.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <examples/imgui_impl_vulkan.h>

#include "Aurora.hpp"
#include "ui/ImGuiLayer.hpp"
#include "ui/imgui_impl_glfw.h"
#include "ui/KeyMappings.hpp"
#include "utils/dbg.h"
#include "utils/Utils.hpp"

static int imGuiContexts = 0;
static ImGuiContext* mainCtx = nullptr;
static VkDescriptorPool vk_descriptorPool = VK_NULL_HANDLE;

ImGuiLayer::ImGuiLayer(AuroraWindow& parentWindow): UILayer(parentWindow) {
	IMGUI_CHECKVERSION();
	
	if (ImGui::GetCurrentContext() != nullptr) {
		ctx = ImGui::CreateContext(ImGui::GetIO().Fonts);
		imGuiGlfw = new ImGuiGlfw(window.window->impl->glfw_window);
	
	} else {
		ctx = ImGui::CreateContext();
		imGuiGlfw = new ImGuiGlfw(window.window->impl->glfw_window);
		
		initShared();
		mainCtx = ctx;
	}
	
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
  
  ImGui::StyleColorsDark();
  imGuiContexts++;
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
			
			auto result = vkCreateDescriptorPool(Aurora.vk2dInstance->impl->GetVulkanDevice(), &pool_info, nullptr, &vk_descriptorPool);
			check(result == VK_SUCCESS, "error creating descriptor pool: " << result);
		}
		
		ImGui_ImplVulkan_InitInfo init_info = { };
		init_info.Instance = Aurora.vk2dInstance->impl->GetVulkanInstance();
		init_info.PhysicalDevice = Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice();
		init_info.Device = Aurora.vk2dInstance->impl->GetVulkanDevice();
		init_info.Queue = Aurora.vk2dInstance->impl->GetPrimaryRenderQueue().GetQueue(); // or secondary_render_queue
		init_info.QueueFamily = Aurora.vk2dInstance->impl->GetPrimaryRenderQueue().GetQueueFamilyIndex();
		init_info.PipelineCache = Aurora.vk2dInstance->impl->GetGraphicsPipelineCache();
		init_info.DescriptorPool = vk_descriptorPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2; // Seems to be unused
		init_info.ImageCount = window.window->impl->swapchain_image_count;
		init_info.CheckVkResultFn = [](VkResult result) {
			if (result != VK_SUCCESS) {
				Aurora.vk2dInstance->impl->Report(result, "Vulkan error in ImGui");
			}
		};
		
		initImGuiVulkan = ImGui_ImplVulkan_Init(&init_info, window.window->impl->vk_render_pass);
		check(initImGuiVulkan, "failed to init ImGui vulkan");
		
		// Upload Fonts
		{
			// Use any command queue
			VkCommandPool command_pool = window.window->impl->vk_command_pool;
			VkCommandBuffer command_buffer = window.window->impl->vk_render_command_buffers[window.window->impl->next_image];
			
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
	if (ctx != nullptr) {
		ImGui::SetCurrentContext(ctx);
		
		for (UIWindow* uiWindow : uiWindows) {
			delete uiWindow;
		}
		
		if (--imGuiContexts == 0) {
			ImGui_ImplVulkan_Shutdown();
			
			if (vk_descriptorPool != VK_NULL_HANDLE) {
				vkDestroyDescriptorPool(Aurora.vk2dInstance->impl->GetVulkanDevice(), vk_descriptorPool, nullptr);
				vk_descriptorPool = VK_NULL_HANDLE;
			}
		}
		
		delete imGuiGlfw;
		
		if (!ctx->FontAtlasOwnedByContext) {
			ImGui::DestroyContext();
		}
		
		if (imGuiContexts == 0) {
			ImGui::SetCurrentContext(mainCtx);
			ImGui::DestroyContext();
			mainCtx = nullptr;
		}
	}
}

void ImGuiLayer::render() {
	ImGui::SetCurrentContext(ctx);
	ImGui_ImplVulkan_NewFrame();
	imGuiGlfw->NewFrame();
	ImGui::NewFrame();
	
	// Record ImGui primitives into command buffer
  {
  	ZoneScopedN("ImGui");
  	
  	for (UIWindow* uiWindow : uiWindows) {
  		if (uiWindow->visible) {
  			uiWindow->render();
  		}
		}
		
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
		
		TracyVkZone(window.tracyVkCtxs[window.window->impl->next_image], window.window->impl->vk_render_command_buffers[window.window->impl->next_image], "ImGui Render");
		ImGui_ImplVulkan_RenderDrawData(draw_data, window.window->impl->vk_render_command_buffers[window.window->impl->next_image]);
  }
}

void ImGuiLayer::addWindow(UIWindow* uiWindow) {
	uiWindows.push_back(uiWindow);
}

void ImGuiLayer::removeWindow(UIWindow* uiWindow) {
	vectorEraseUnorderedVal(uiWindows, uiWindow);
	delete uiWindow;
}

template<aUIWindow T>
void ImGuiLayer::showWindow() {
	for (UIWindow* uiWindow : uiWindows) {
		if (dynamic_cast<T*>(uiWindow)) {
			uiWindow->visible = true;
			return;
		}
	}
	
	throw std::invalid_argument("No ui window of type " + type_name<T>());
}

template<aUIWindow T>
void ImGuiLayer::hideWindow() {
	for (UIWindow* uiWindow : uiWindows) {
		if (dynamic_cast<T*>(uiWindow)) {
			uiWindow->visible = false;
			return;
		}
	}
	
	throw std::invalid_argument("No ui window of type " + type_name<T>());
}

bool ImGuiLayer::eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	if (ctx->IO.WantCaptureKeyboard) {
		ImGui::SetCurrentContext(ctx);
		imGuiGlfw->KeyCallback(nullptr, static_cast<int>(button), static_cast<int>(scancode), static_cast<int>(action), static_cast<int>(modifier_keys));
		return true;
	}
	
	return false;
}

bool ImGuiLayer::eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
	if (ctx->IO.WantCaptureKeyboard) {
		ImGui::SetCurrentContext(ctx);
		imGuiGlfw->CharCallback(nullptr, static_cast<unsigned int>(character));
		return true;
	}
	
	return false;
}

bool ImGuiLayer::eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	if (ctx->IO.WantCaptureMouse) {
		ImGui::SetCurrentContext(ctx);
		imGuiGlfw->MouseButtonCallback(nullptr, static_cast<int>(button), static_cast<int>(action), static_cast<int>(modifier_keys));
		return true;
	}
	
	return false;
}

bool ImGuiLayer::eventScroll(vk2d::Vector2d scroll) {
	if (ctx->IO.WantCaptureMouse) {
		ImGui::SetCurrentContext(ctx);
		imGuiGlfw->ScrollCallback(nullptr, scroll.x, scroll.y);
		return true;
	}
	
	return false;
}
