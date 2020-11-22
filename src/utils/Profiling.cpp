/*
 * Profiling.cpp
 *
 *  Created on: 11 Nov 2020
 *      Author: exuvo
 */

#include "utils/Profiling.hpp"

void ProfilerEvents::start(nanoseconds time, std::string name) {
	events.push_back(ProfilerEvent().start(time, name));
}

void ProfilerEvents::start(const char* name) {
	events.push_back(ProfilerEvent().start(name));
}

void ProfilerEvents::start(std::string name) {
	events.push_back(ProfilerEvent().start(name));
}

void ProfilerEvents::end(nanoseconds time) {
	events.push_back(ProfilerEvent().end(time));
}

void ProfilerEvents::clear() {
	events.clear();
}
