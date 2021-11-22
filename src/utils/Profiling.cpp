/*
 * Profiling.cpp
 *
 *  Created on: 11 Nov 2020
 *      Author: exuvo
 */

#include "utils/Profiling.hpp"

void ProfilerEvents::start(nanoseconds time, std::string name) {
	events.emplace_back(time, name);
}

void ProfilerEvents::start(const char* name) {
	events.emplace_back(name);
}

void ProfilerEvents::start(const char* name, size_t length) {
	events.emplace_back(name, length);
}

void ProfilerEvents::start(std::string name) {
	events.emplace_back(name);
}

void ProfilerEvents::end(nanoseconds time) {
	events.emplace_back(time);
}

const ProfilerEvent& ProfilerEvents::operator[](size_t idx) const {
	return events[idx];
}

size_t ProfilerEvents::size() const {
	return events.size();
}

void ProfilerEvents::clear() {
	events.clear();
}
