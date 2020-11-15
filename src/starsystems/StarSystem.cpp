/*
 * StarSystem.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include <thread>
#include <iostream>

#include "Tracy.hpp"

#include "StarSystem.hpp"

void StarSystem::init() {
	
}

void StarSystem::update(uint32_t deltaGameTime) {
	std::this_thread::sleep_for(50ms);
	LOG4CXX_INFO(log, "starsystem " << name << " took " << updateTime.count() << "ns");
//	std::cout << "starsystem " << name << " (" << galacticEntityID << ")" << std::endl;
}

