#include <iostream>
#include <thread>

#include "Tracy.hpp"
#include "log4cxx/logger.h"
#include "log4cxx/xml/domconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include "galaxy/Galaxy.hpp"
#include "ui/AuroraWindow.hpp"
#include "Aurora.hpp"

using namespace std;
using namespace log4cxx;

//TODO Port simulation, test performance via console/tracy

AuroraGlobal Aurora;

void VK2D_APIENTRY VK2D_log(vk2d::ReportSeverity severity, std::string_view message) {
	std::cout << message << std::endl;
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
	vector<Empire> empires { Empire("player1") };
	Galaxy galaxy(empires, starSystems);
	Aurora.galaxy = &galaxy;

	galaxy.init();
	
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
	
	AuroraWindow* window = new AuroraWindow();
	Aurora.windows.push_back(window);
	
	cout <<  "running" << endl;

	uint32_t frameRate = 60;
	nanoseconds frameDelay = 1s / frameRate;
	
	nanoseconds accumulator = 0s;
	nanoseconds lastRun = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
	
	while(!Aurora.shutdown){
		
		// make each window its own thread? maybe with separate vk2dInstance?
		while (true) {
			nanoseconds now = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
			accumulator += now - lastRun;
			lastRun = now;
			
			if (accumulator >= frameDelay) {
				accumulator -= frameDelay;
				
				if (accumulator > frameDelay) {
					accumulator = accumulator % frameDelay;
				}
				
				break;
				
			} else if (Aurora.shutdown) {
				break;
				
			} else {
				
				milliseconds sleepTime = duration_cast<milliseconds>(frameDelay - accumulator);
				
				if (sleepTime >= 10ms) {
					std::this_thread::sleep_for(5ms);
					break;
					
				} else if (sleepTime > 1ms) {
					std::this_thread::sleep_for(sleepTime - 1ms);
					
				} else {
					std::this_thread::yield();
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
					delete window;
					i--;
				}
			}
			
		} catch (const std::exception& e) {
			LOG4CXX_ERROR(log, "Exception in update: " << e.what());
		}
	}
	
	cout << "end" << flush;
	
	delete Aurora.vk2dInstance;

	tracy::ShutdownProfiler();
	return 0;
}
