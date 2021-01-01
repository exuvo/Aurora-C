#include <iostream>
#include <thread>

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

using namespace std;
using namespace log4cxx;

//TODO Port simulation, test performance

AuroraGlobal Aurora;

void VK2D_log(vk2d::ReportSeverity severity, std::string_view message) {
	LoggerPtr log = Logger::getLogger("aurora.ui");
	
	if (severity == vk2d::ReportSeverity::CRITICAL_ERROR) {
		LOG4CXX_FATAL(log, message);
	} else if (severity == vk2d::ReportSeverity::DEVICE_LOST) {
		LOG4CXX_FATAL(log, message);
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

int main(int argc, char **argv) {
	tracy::StartupProfiler();
	tracy::SetThreadName("startup");

	log4cxx::xml::DOMConfigurator::configure("log4j.xml");
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
	
	Aurora.vk2dInstance = vk2d::CreateInstance(instance_create_info).release();
	
	if (!Aurora.vk2dInstance) {
		LOG4CXX_ERROR(log, "failed to create vk2d instance");
		return 1;
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
	
	{
		auto window = new AuroraWindow();
		window->setMainLayer(new StarSystemLayer(*window, galaxy->systems[0]));
		window->addLayer(new ImGuiLayer(*window));
		Aurora.windows.push_back(window);
	}
	
	cout <<  "running" << endl;

	uint32_t targetFrameRate = 60;
	nanoseconds targetFrameDelay = duration_cast<nanoseconds>(1s) / targetFrameRate;
	assert(targetFrameDelay > 0ns);
	
	nanoseconds accumulator = 0s;
	nanoseconds lastRun = getNanos();
	uint16_t frames = 0;
	nanoseconds frameCounterStart = lastRun;
	
	while(!Aurora.shutdown){
		
		// make each window its own thread? maybe with separate vk2dInstance?
		if (Aurora.settings.render.vsync) {
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

	tracy::ShutdownProfiler();
	exit(0);
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
