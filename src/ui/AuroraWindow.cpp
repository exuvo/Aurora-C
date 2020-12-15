/*
 * AuroraWindow.cpp
 *
 *  Created on: Dec 10, 2020
 *      Author: exuvo
 */

#include <iostream>

#include <VK2D.h>
#include <Interface/InstanceImpl.h>
#include <Interface/WindowImpl.h>

#include <imgui.h>
#include <examples/imgui_impl_vulkan.h>
#include <examples/imgui_impl_glfw.h>

#include "AuroraWindow.hpp"
#include "Aurora.hpp"
#include "utils/Utils.hpp"

using namespace std::chrono;

static VkDescriptorPool vk_descriptorPool = VK_NULL_HANDLE;

AuroraWindow::AuroraWindow() {
	
	std::cout << "creating window" << std::endl;
	
	vk2d::WindowCreateInfo window_create_info{};
	window_create_info.size = { 512, 512 };
	window_create_info.coordinate_space = vk2d::RenderCoordinateSpace::TEXEL_SPACE_CENTERED;
	window_create_info.event_handler = this;
//		window_create_info.fullscreen_monitor = ?
//		window_create_info.fullscreen_refresh_rate = ?
//		window_create_info.vsync = ?
	window = Aurora.vk2dInstance->CreateOutputWindow(window_create_info);
	
	if (!window)  {
		LOG4CXX_ERROR(log, "failed to create window");
		throw std::runtime_error("failed to create window");
	}
	
	for(size_t i=0; i < window->impl->vk_render_command_buffers.size(); i++) {
		TracyVkCtx tmp = TracyVkContext(Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice(),
		                                    Aurora.vk2dInstance->impl->GetVulkanDevice(),
		                                    Aurora.vk2dInstance->impl->GetPrimaryRenderQueue().GetQueue(),
		                                    window->impl->vk_render_command_buffers[i]);
		tracyVkCtxs.push_back(tmp);
	}
	
	IMGUI_CHECKVERSION();
	
	if (ImGui::GetCurrentContext() != nullptr) {
		imGuiCtx = ImGui::CreateContext(ImGui::GetIO().Fonts);
	} else {
		imGuiCtx = ImGui::CreateContext();
	}
	
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_IsSRGB;
  
  ImGui::StyleColorsDark();
  
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
			if (result != VK_SUCCESS) {
				Aurora.vk2dInstance->impl->Report(result, "Unable to create descriptor pool");
			}
  }
  
  bool ok = ImGui_ImplGlfw_InitForVulkan(window->impl->glfw_window, true);
  ImGui_ImplVulkan_InitInfo init_info = {};
  init_info.Instance = Aurora.vk2dInstance->impl->GetVulkanInstance();
	init_info.PhysicalDevice = Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice();
	init_info.Device = Aurora.vk2dInstance->impl->GetVulkanDevice();
	init_info.Queue = Aurora.vk2dInstance->impl->GetPrimaryRenderQueue().GetQueue(); // or secondary_render_queue
	init_info.QueueFamily = Aurora.vk2dInstance->impl->GetPrimaryRenderQueue().GetQueueFamilyIndex();
	init_info.PipelineCache = Aurora.vk2dInstance->impl->GetGraphicsPipelineCache();
	init_info.DescriptorPool = vk_descriptorPool;
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2; // Seems to be unused
	init_info.ImageCount = window->impl->swapchain_image_count;
	init_info.CheckVkResultFn = [](VkResult result){
		if (result != VK_SUCCESS) {
			Aurora.vk2dInstance->impl->Report(result, "Vulkan error in ImGui");
		}
	};
	ImGui_ImplVulkan_Init(&init_info, window->impl->vk_render_pass);
	
	// Upload Fonts
	{
			// Use any command queue
		VkCommandPool command_pool = window->impl->vk_command_pool;
		VkCommandBuffer command_buffer = window->impl->vk_render_command_buffers[window->impl->next_image];
		
		auto result = vkResetCommandPool(init_info.Device, command_pool, 0);
		if (result != VK_SUCCESS) {
			Aurora.vk2dInstance->impl->Report(result, "Cannot reset command pool for fonts");
			throw std::runtime_error("failed to create window");
		}
		
		VkCommandBufferBeginInfo begin_info = { };
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		result = vkBeginCommandBuffer(command_buffer, &begin_info);
		if (result != VK_SUCCESS) {
			Aurora.vk2dInstance->impl->Report(result, "Cannot begin command pool for fonts");
			throw std::runtime_error("failed to create window");
		}
		
		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);
		
		VkSubmitInfo end_info = { };
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		result = vkEndCommandBuffer(command_buffer);
		if (result != VK_SUCCESS) {
			Aurora.vk2dInstance->impl->Report(result, "Cannot end command pool for fonts");
			throw std::runtime_error("failed to create window");
		}
		
		result = vkQueueSubmit(init_info.Queue, 1, &end_info, VK_NULL_HANDLE);
		if (result != VK_SUCCESS) {
			Aurora.vk2dInstance->impl->Report(result, "Cannot submit queue for fonts");
			throw std::runtime_error("failed to create window");
		}
		
		result = vkDeviceWaitIdle(init_info.Device);
		if (result != VK_SUCCESS) {
			Aurora.vk2dInstance->impl->Report(result, "Error waiting on device idle for fonts");
			throw std::runtime_error("failed to create window");
		}
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

void AuroraWindow::render() {
	ZoneScoped;
	if(!window->BeginRender()) {
		LOG4CXX_ERROR(log, "Error rendering window begin");
		return;
	}
	
	ImGui::SetCurrentContext(imGuiCtx);
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	float x = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count() % (100 + window->GetSize().x) - window->GetSize().x / 2 - 100;
	
	{
		ZoneScopedN("Draw");
		TracyVkZone(tracyVkCtxs[window->impl->next_image], window->impl->vk_render_command_buffers[window->impl->next_image], "Render");
		
		window->DrawRectangle(
			{{x, 0}, {x + 100,100}},
			true,
			vk2d::Colorf( 1.0f, 0.0f, 0.0f, 1.0f )
		);
	}
	
  // Record ImGui primitives into command buffer
  {
  	ZoneScopedN("ImGui");
  	
		ImGui::Begin("Hello, world!");
		ImGui::Text("test");
		ImGui::End();
		
		ImGui::Render();
		ImDrawData* draw_data = ImGui::GetDrawData();
  
		TracyVkZone(tracyVkCtxs[window->impl->next_image], window->impl->vk_render_command_buffers[window->impl->next_image], "ImGui Render");
		ImGui_ImplVulkan_RenderDrawData(draw_data, window->impl->vk_render_command_buffers[window->impl->next_image]);
  }
	
	if(!window->EndRender()) {
		LOG4CXX_ERROR(log, "Error rendering window end");
		return;
	}
	FrameMark
}

AuroraWindow::~AuroraWindow() {
	if (window) {
		
		if (imGuiCtx != nullptr) {
			ImGui::SetCurrentContext(imGuiCtx);
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext(); //TODO if first window kills still in use shared fonts
		}
		
		for (TracyVkCtx tracyVkCtx : tracyVkCtxs) {
			TracyVkDestroy(tracyVkCtx);
		}
		
		Aurora.vk2dInstance->DestroyOutputWindow(window);
		window = nullptr;
	}
	
	vectorEraseUnorderedVal(Aurora.windows, this);
	
	if (Aurora.windows.size() == 0) {
		Aurora.shutdown = true;
		
		if (vk_descriptorPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(Aurora.vk2dInstance->impl->GetVulkanDevice(), vk_descriptorPool, nullptr);
		}
	}
}

void AuroraWindow::EventMouseButton(vk2d::Window*	window, vk2d::MouseButton	button, vk2d::ButtonAction action,
		                                vk2d::ModifierKeyFlags modifier_keys) {
	
}

void AuroraWindow::EventWindowPosition(vk2d::Window* window, vk2d::Vector2i position) {
}

void AuroraWindow::EventScreenshot(vk2d::Window* window, const std::filesystem::__cxx11::path& screenshot_path,
                                   const vk2d::ImageData& screenshot_data, bool success,
                                   const std::string& error_message) {
}

void AuroraWindow::EventCursorEnter(vk2d::Window* window, bool entered) {
//	std::cout << "window cursorEnter " << entered << std::endl;
}

void AuroraWindow::EventWindowFocus(vk2d::Window* window, bool focused) {
	std::cout << "window focus " << focused << std::endl;
}

void AuroraWindow::EventCursorPosition(vk2d::Window* window, vk2d::Vector2d position) {
}

void AuroraWindow::EventFileDrop(vk2d::Window* window,
    std::vector<std::filesystem::__cxx11::path, std::allocator<std::filesystem::__cxx11::path> > files) {
}

void AuroraWindow::EventScroll(vk2d::Window* window, vk2d::Vector2d scroll) {
	std::cout << "window scroll" << std::endl;
}

void AuroraWindow::EventWindowIconify(vk2d::Window* window, bool iconified) {
	std::cout << "window iconify " << iconified << std::endl;
}

void AuroraWindow::EventWindowClose(vk2d::Window* window) {
	std::cout << "window close" << std::endl;
	window->CloseWindow();
}

void AuroraWindow::EventCharacter(vk2d::Window* window, uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
	if (character == 'n') {
		Aurora.windows.push_back(new AuroraWindow());
	}
}

void AuroraWindow::EventWindowSize(vk2d::Window* window, vk2d::Vector2u size) {
//	std::cout << "window resize" << std::endl;
}

void AuroraWindow::EventWindowMaximize(vk2d::Window* window, bool maximized) {
	std::cout << "window maximized " << maximized << std::endl;
}

void AuroraWindow::EventKeyboard(vk2d::Window* window, vk2d::KeyboardButton button, int32_t scancode,
                                 vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
}
