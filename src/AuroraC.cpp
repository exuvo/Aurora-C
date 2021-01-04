#include <iostream>
#include <thread>

#include <Interface/WindowImpl.h>

#include <Tracy.hpp>

#include <log4cxx/logger.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/helpers/exception.h>

#include "galaxy/Galaxy.hpp"
#include "ui/AuroraWindow.hpp"
#include "ui/StarSystemLayer.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "ui/KeyMappings.hpp"
#include "Aurora.hpp"
#include "ui/ShaderTestLayer.hpp"

#define GLFW_EXPOSE_NATIVE_X11 true
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_xlib.h>
#include <vulkan/vulkan_xlib_xrandr.h>

using namespace std;
using namespace log4cxx;

AuroraGlobal Aurora;

bool hasVK_EXT_display_control = false;
bool hasVK_EXT_display_surface_counter = false;
bool hasVK_KHR_display = false;
bool hasVK_EXT_acquire_xlib_display = false;
bool hasVK_EXT_direct_mode_display = false;

void VK2D_log(vk2d::ReportSeverity severity, std::string_view message) {
	LoggerPtr log = Logger::getLogger("aurora.ui");
	
	if (severity == vk2d::ReportSeverity::CRITICAL_ERROR) {
		LOG4CXX_FATAL(log, message << endl << getCurrentStacktrace());
	} else if (severity == vk2d::ReportSeverity::DEVICE_LOST) {
		LOG4CXX_FATAL(log, message << endl << getCurrentStacktrace());
	} else if (severity == vk2d::ReportSeverity::NON_CRITICAL_ERROR) {
		LOG4CXX_ERROR(log, message);
	} else if (severity == vk2d::ReportSeverity::WARNING || severity == vk2d::ReportSeverity::PERFORMANCE_WARNING) {
		LOG4CXX_WARN(log, message);
	} else if (severity == vk2d::ReportSeverity::INFO) {
		LOG4CXX_INFO(log, message);
	} else if (severity == vk2d::ReportSeverity::VERBOSE) {
		LOG4CXX_DEBUG(log, message);
	}
}

void VK2D_instance_extensions(const std::vector<VkExtensionProperties>& available_instance_extensions, std::vector<const char*>& instance_extensions) {
	LoggerPtr log = Logger::getLogger("aurora.ui");
	
	for (const VkExtensionProperties& extensionProperties : available_instance_extensions) {
		if (strcmp(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME, extensionProperties.extensionName) == 0) {
			hasVK_EXT_display_surface_counter = true;
			instance_extensions.push_back(VK_EXT_DISPLAY_SURFACE_COUNTER_EXTENSION_NAME);
			LOG4CXX_INFO(log, "Vulkan has VK_EXT_display_surface_counter extension");
			
		} else if (strcmp(VK_KHR_DISPLAY_EXTENSION_NAME, extensionProperties.extensionName) == 0) {
			hasVK_KHR_display = true;
			instance_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
			LOG4CXX_INFO(log, "Vulkan has VK_KHR_display extension");
			
		} else if (strcmp(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME, extensionProperties.extensionName) == 0) {
			hasVK_EXT_acquire_xlib_display = true;
			instance_extensions.push_back(VK_EXT_ACQUIRE_XLIB_DISPLAY_EXTENSION_NAME);
			LOG4CXX_INFO(log, "Vulkan has VK_EXT_acquire_xlib_display extension");
			
		} else if (strcmp(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME, extensionProperties.extensionName) == 0) {
			hasVK_EXT_direct_mode_display = true;
			instance_extensions.push_back(VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME);
			LOG4CXX_INFO(log, "Vulkan has VK_EXT_direct_mode_display extension");
		}
	}
}

void VK2D_device_extensions(const std::vector<VkExtensionProperties>& available_device_extensions, std::vector<const char*>& device_extensions) {
	LoggerPtr log = Logger::getLogger("aurora.ui");
	
	for (const VkExtensionProperties& extensionProperties : available_device_extensions) {
		if (strcmp(VK_EXT_DISPLAY_CONTROL_EXTENSION_NAME, extensionProperties.extensionName) == 0) {
			hasVK_EXT_display_control = true;
			device_extensions.push_back(VK_EXT_DISPLAY_CONTROL_EXTENSION_NAME);
			LOG4CXX_INFO(log, "Vulkan has VK_EXT_display_control extension");
			break;
		}
	}
}

PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vk_GetPhysicalDeviceSurfaceCapabilities2EXT = VK_NULL_HANDLE;
PFN_vkGetRandROutputDisplayEXT vk_GetRandROutputDisplayEXT = VK_NULL_HANDLE;

std::thread* vsyncThread = nullptr;
void vsyncWorker(VkDisplayKHR vkDisplay);

int main(int argc, char **argv) {
	tracy::StartupProfiler();
	tracy::SetThreadName("startup");

	log4cxx::xml::DOMConfigurator::configure("assets/log4j.xml");
	LoggerPtr log = Logger::getLogger("aurora");
	LOG4CXX_FATAL(log, "### Starting ###");

	cout <<  "starting galaxy" << endl << flush;

	vector<StarSystem*> starSystems { new StarSystem("test")};
//	vector<StarSystem*> starSystems { new StarSystem("a"), new StarSystem("b"), new StarSystem("c"), new StarSystem("d"), new StarSystem("e") };
	vector<Empire> empires { Empire("gaia"), Empire("player1") };
	vector<Player> players { Player("local") };
	Galaxy* galaxy = new Galaxy(empires, starSystems, players);
	Aurora.galaxy = galaxy;
	Player::current = &galaxy->players[0];
	Player::current->empire = &galaxy->empires[1];

	galaxy->init();
	
	LOG4CXX_INFO(log, "creating vk2d instance");
	std::cout <<  "creating vk2d instance" << std::endl;
	
	vk2d::InstanceCreateInfo instance_create_info {};
	instance_create_info.application_name = "Aurora C";
	instance_create_info.application_version = {0, 1, 0};
//	instance_create_info.engine_name = "";
//	instance_create_info.engine_version = {0, 0, 0};
	instance_create_info.report_function = VK2D_log;
	instance_create_info.instance_extensions_function = VK2D_instance_extensions;
	instance_create_info.device_extensions_function = VK2D_device_extensions;
	
	Aurora.vk2dInstance = vk2d::CreateInstance(instance_create_info).release();
	
	if (!Aurora.vk2dInstance) {
		LOG4CXX_ERROR(log, "failed to create vk2d instance");
		return 1;
	}
	
	if (hasVK_EXT_display_surface_counter) {
		vk_GetPhysicalDeviceSurfaceCapabilities2EXT = (PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT) vkGetInstanceProcAddr(Aurora.vk2dInstance->impl->GetVulkanInstance(), "vkGetPhysicalDeviceSurfaceCapabilities2EXT");
	}
	
	if (hasVK_EXT_acquire_xlib_display) {
		vk_GetRandROutputDisplayEXT = (PFN_vkGetRandROutputDisplayEXT) vkGetInstanceProcAddr(Aurora.vk2dInstance->impl->GetVulkanInstance(), "vkGetRandROutputDisplayEXT");
	}
	
	KeyMappings::loadAllDefaults();
	
	// main font should have fixed width numbers
	Aurora.assets.font = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/LiberationSans-Regular.ttf"), 18, true, '?');
	Aurora.assets.font5 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/5pxbus.ttf"), 5, true, '?');
	Aurora.assets.font6 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/6pxbus.ttf"), 6, true, '?');
	Aurora.assets.font7 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/7pxbus.ttf"), 7, true, '?');
	Aurora.assets.font8 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/8pxbus.ttf"), 8, true, '?');
	Aurora.assets.font9 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/9pxbus.ttf"), 9, true, '?');
	Aurora.assets.font11 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/11pxbus.ttf"), 11, true, '?');
	Aurora.assets.font13 = Aurora.vk2dInstance->GetResourceManager()->LoadFontResource(std::filesystem::relative("assets/fonts/13pxbus.ttf"), 13, true, '?');
	
	VkFence vsyncFence = VK_NULL_HANDLE;
//	VkDisplayKHR vkDisplay {};
	
	{
		auto window = new AuroraWindow();
		window->setMainLayer(new StarSystemLayer(*window, galaxy->systems[0]));
		window->addLayer(new ImGuiLayer(*window));
		window->addLayer(new ShaderTestLayer(*window));
		Aurora.windows.push_back(window);
		
		if (hasVK_EXT_display_surface_counter) {
			
			VkSurfaceCapabilities2EXT surfaceCaps {};
			surfaceCaps.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_EXT;
			
			VkResult res = vk_GetPhysicalDeviceSurfaceCapabilities2EXT(window->window->impl->vk_physical_device, window->window->impl->vk_surface, &surfaceCaps);
			
			if (res == VK_SUCCESS) {
				cout <<  "vsync surface " << surfaceCaps.supportedSurfaceCounters << " ";
				
				if (surfaceCaps.supportedSurfaceCounters & VK_SURFACE_COUNTER_VBLANK_EXT) {
					cout <<  " vblank ext";
				}
				
				cout << endl;
			} else {
				LOG4CXX_ERROR(log, "unable to get surface capabilites: " << res);
			}
			
			hasVK_EXT_display_surface_counter = false;
		}
		
		if (hasVK_EXT_acquire_xlib_display) {
			
			Display* x11Display = glfwGetX11Display();
			
			if (x11Display == nullptr) {
				LOG4CXX_ERROR(log, "x11Display null");
			} else {
			
				GLFWmonitor* glfwMonitor = glfwGetWindowMonitor(window->window->impl->glfw_window);
				
				if (glfwMonitor == nullptr) {
					cout <<  "glfwGetWindowMonitor null" << endl;
					glfwMonitor = glfwGetPrimaryMonitor();
				}
				
				if (glfwMonitor == nullptr) {
					LOG4CXX_ERROR(log, "glfwMonitor null");
				} else {
				
					RROutput x11RandRoutputID = glfwGetX11Monitor(glfwMonitor);
					VkDisplayKHR vkDisplay {};
					
					auto res = vk_GetRandROutputDisplayEXT(Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice(), x11Display, x11RandRoutputID, &vkDisplay);
					if (res == VK_SUCCESS) {
						cout <<  "current X11 vkDisplay " << vkDisplay << endl;
						
						vsyncThread = new std::thread(vsyncWorker, vkDisplay);
						
					} else {
						LOG4CXX_ERROR(log, "unable to get current X11 display: " << res);
					}
				}
			}
		}
		
		if (hasVK_EXT_display_control && hasVK_KHR_display) {
			
			uint32_t displays = 0;
			auto res = vkGetPhysicalDeviceDisplayPropertiesKHR(Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice(), &displays, nullptr);
			
			if (res == VK_SUCCESS) {
				cout <<  "displays " << displays << endl;
			} else {
				LOG4CXX_ERROR(log, "unable to get displays: " << res);
			}
			
			if (displays > 0) {
				std::vector<VkDisplayPropertiesKHR> displayProperties (displays);
				res = vkGetPhysicalDeviceDisplayPropertiesKHR(Aurora.vk2dInstance->impl->GetVulkanPhysicalDevice(), &displays, displayProperties.data());
				if (res != VK_SUCCESS) {
					LOG4CXX_ERROR(log, "unable to get displayProperties: " << res);
				}
				
				if (vsyncThread == nullptr) {
					vsyncThread = new std::thread(vsyncWorker, displayProperties[0].display);
				}
			}
		}
	}
	
	cout <<  "running" << endl;
	
	nanoseconds targetFrameDelay = duration_cast<nanoseconds>(1s) / Aurora.settings.render.targetFrameRate;
	assert(targetFrameDelay > 0ns);
	
	nanoseconds accumulator = 0s;
	nanoseconds lastRun = getNanos();
	uint16_t frames = 0;
	nanoseconds frameCounterStart = lastRun;
	
	while(!Aurora.shutdown){
		
		// make each window its own thread? maybe with separate vk2dInstance?
		
		if (!Aurora.settings.render.vsync) {
			while (true) {
				nanoseconds now = getNanos();
				accumulator += now - lastRun;
				lastRun = now;
				
				if (accumulator >= targetFrameDelay) {
					accumulator -= targetFrameDelay;
					
					if (accumulator > targetFrameDelay) {
						accumulator = accumulator % targetFrameDelay;
					}
					
					break;
					
				} else if (Aurora.shutdown) {
					break;
					
				} else {
					
					milliseconds sleepTime = duration_cast<milliseconds>(targetFrameDelay - accumulator);
					
					if (sleepTime >= 10ms) {
						std::this_thread::sleep_for(8ms);
						
					} else if (sleepTime > 1ms) {
						std::this_thread::sleep_for(sleepTime - 1ms);
						
					} else {
						std::this_thread::yield();
					}
				}
			}
		}
		
		try {
			Aurora.vk2dInstance->Run();
			
			for (AuroraWindow* window : Aurora.windows) {
				window->render();
				
				if (hasVK_EXT_display_control) {
					
					// vkGetDeviceProcAddr is slightly faster but specific to each device: https://stackoverflow.com/questions/35504545/vulkan-difference-between-vkgetinstanceprocaddress-and-vkgetdeviceprocaddress
					PFN_vkGetSwapchainCounterEXT vk_GetSwapchainCounterEXT = VK_NULL_HANDLE;
					vk_GetSwapchainCounterEXT = (PFN_vkGetSwapchainCounterEXT) vkGetDeviceProcAddr(Aurora.vk2dInstance->impl->GetVulkanDevice(), "vkGetSwapchainCounterEXT");
					
					uint64_t counter;
					VkResult res = vk_GetSwapchainCounterEXT(window->window->impl->vk_device, window->window->impl->vk_swapchain, VK_SURFACE_COUNTER_VBLANK_EXT, &counter);
					
					if (res == VK_SUCCESS) {
						cout <<  "vsync counter " << counter << endl;
					} else {
						LOG4CXX_ERROR(log, "unable to get vsync counter: " << res);
						hasVK_EXT_display_control = false;
					}
				}
			}
			
			for (size_t i=0; i < Aurora.windows.size(); i++) {
				AuroraWindow* window = Aurora.windows[i];
				
				if (window->window->ShouldClose()) {
					vectorEraseUnorderedIdx(Aurora.windows, i);
					delete window;
					i--;
				}
			}
			
		} catch (const std::exception& e) {
			LOG4CXX_ERROR(log, "Exception in update: " << e.what());
		}
		
		nanoseconds now = getNanos();
		if (now - frameCounterStart >= 1s) {
			Aurora.fps = frames;
			frames = 0;
			frameCounterStart = now;
		}
		
		frames++;
	}
	
	cout << "shutdown" << flush;
	
	if (Aurora.galaxy != nullptr) {
		Aurora.galaxy->shutdown = true;
		std::unique_lock<std::mutex> lock(Aurora.galaxy->galaxyThreadMutex);
		Aurora.galaxy->galaxyThreadCondvar.notify_one();
	}
	
	delete Aurora.vk2dInstance;
	
	if (Aurora.galaxy != nullptr) {
		Aurora.galaxy->galaxyThread->join();
	}
	
	if (vsyncThread != nullptr) {
		vsyncThread->join();
		delete vsyncThread;
	}

	tracy::ShutdownProfiler();
	exit(0);
}

//TODO time well against vsync using VK_EXT_display_control and VK_EXT_display_surface_counter
		// https://stackoverflow.com/questions/61031850/how-to-get-the-next-frame-presentation-time-in-vulkan
		// https://github.com/KhronosGroup/Vulkan-Docs/issues/370
		// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VK_EXT_display_control.html
		// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VK_EXT_display_surface_counter.html
		// using OpenXR https://www.khronos.org/registry/OpenXR/specs/1.0/html/xrspec.html#frame-synchronization

nanoseconds lastVsync = getNanos();
void vsyncWorker(VkDisplayKHR vkDisplay) {
	
//	cout <<  "vsync thread started" << endl;
	
	LoggerPtr log = Logger::getLogger("aurora");
	VkFence vsyncFence = VK_NULL_HANDLE;
	PFN_vkRegisterDisplayEventEXT vk_RegisterDisplayEventEXT = (PFN_vkRegisterDisplayEventEXT) vkGetDeviceProcAddr(Aurora.vk2dInstance->impl->GetVulkanDevice(), "vkRegisterDisplayEventEXT");
	
//	VkAllocationCallbacks allocator { nullptr, 
//		[](void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope){
//			printf("vk allocate: size: %lu, alignment: %lu, allocationScope: %d", size, alignment, allocationScope);
//			void* ptr = malloc(size); // Rely on mallocs natural 16 alignment on 64bit OS
////			void* ptr = aligned_alloc(size, alignment);
//			memset(ptr, 0, size);
//			printf(", return 0x%p \n", ptr);
//			return ptr;  
//		},
//		[](void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope){
//			printf("vk reallocate: size %lu, alignment %lu, allocationScope %d \n", size, alignment, allocationScope);
//			return realloc(pOriginal, size);
//		}, 
//		[](void* pUserData, void* pMemory){
//			printf("vk free: 0x%p\n", pMemory);
//			free(pMemory);
//		},
//		[](void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope){
//			
//		},
//		[](void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope){
//			
//		}
//	};
	
	while (!Aurora.shutdown) {
		std::this_thread::sleep_for(100ms);
		lastVsync = getNanos();
		
		VkFenceCreateInfo fence_create_info {};
		fence_create_info.sType		= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.pNext		= nullptr;
		fence_create_info.flags		= 0;
		
		// Has to be recreated every vsync register according to https://github.com/KhronosGroup/Vulkan-Docs/issues/370#issuecomment-325702314
		VkResult res = vkCreateFence(
			Aurora.vk2dInstance->impl->GetVulkanDevice(),
			&fence_create_info,
			nullptr,
			&vsyncFence
		);
		if (res != VK_SUCCESS ) {
			LOG4CXX_ERROR(log, "unable to create vsync fence: " << res);
			return;
		}
		
		VkDisplayEventInfoEXT displayEventInfo {};
		displayEventInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_EVENT_INFO_EXT;
		displayEventInfo.displayEvent = VK_DISPLAY_EVENT_TYPE_FIRST_PIXEL_OUT_EXT;
		
		res = vk_RegisterDisplayEventEXT(Aurora.vk2dInstance->impl->GetVulkanDevice(), vkDisplay, &displayEventInfo, nullptr, &vsyncFence);
//		res = vk_RegisterDisplayEventEXT(Aurora.vk2dInstance->impl->GetVulkanDevice(), vkDisplay, &displayEventInfo, &allocator, &vsyncFence);
		
		if (res == VK_SUCCESS) {
			cout <<  "registered vsync event"<< endl;
			
			res = vkWaitForFences(Aurora.vk2dInstance->impl->GetVulkanDevice(), 1, &vsyncFence, VK_TRUE, duration_cast<nanoseconds>(250ms).count());
			
//			vkDestroyFence(Aurora.vk2dInstance->impl->GetVulkanDevice(), vsyncFence, nullptr);
//			vsyncFence = VK_NULL_HANDLE;
			
			if (res == VK_SUCCESS) {
				nanoseconds now = getNanos();
				cout <<  "vsync signaled " << duration_cast<milliseconds>(now - lastVsync).count() << endl;
				lastVsync = now;
				
			} else if (res == VK_TIMEOUT ) {
				cout <<  "vsync not signaled " << duration_cast<milliseconds>(getNanos() - lastVsync).count() << endl;
				
			} else {
				LOG4CXX_ERROR(log, "unable to get vsync fence status: " << res);
				return;
			}
			
		} else {
			LOG4CXX_ERROR(log, "unable to register vsync listener: " << res);
			
			if (res != VK_ERROR_OUT_OF_HOST_MEMORY) { // Crashes
				vkDestroyFence(Aurora.vk2dInstance->impl->GetVulkanDevice(), vsyncFence, nullptr);
			}
			vsyncFence = VK_NULL_HANDLE;
			return;
		}
	}
}

void* operator new(std::size_t count) {
	auto ptr = malloc(count);
	TracySecureAlloc(ptr, count);
	return ptr;
}

void operator delete(void* ptr) noexcept {
	TracySecureFree(ptr);
	free(ptr);
}

void operator delete(void* ptr, size_t size) noexcept {
	TracySecureFree(ptr);
	free(ptr);
}
