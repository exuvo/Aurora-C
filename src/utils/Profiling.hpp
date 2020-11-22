/*
 * Player.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_PROFILING_HPP_
#define SRC_PROFILING_HPP_

#include <string>
#include <cstring>
#include <chrono>

#include "utils/Bag.hpp"
#include "utils/Utils.hpp"

using namespace std::chrono;

class ProfilerEvent {
	public:
		nanoseconds time = 0ns;
		char name[50]; // Set to start event, null to end previous
		
		auto start(const char* name) -> ProfilerEvent& {
			return start(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()), name);
		}
		
		auto start(std::string name) -> ProfilerEvent& {
			return start(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()), name);
		}
		
		auto start(nanoseconds time, const char* name_in) -> ProfilerEvent& {
			ProfilerEvent::time = time;
			strncpy(name, name_in, ARRAY_LEN(name));
			
			return *this;
		}
		
		auto start(nanoseconds time, std::string name_in) -> ProfilerEvent& {
			ProfilerEvent::time = time;
			name_in.copy(name, ARRAY_LEN(name));
			
			return *this;
		}
		
		auto end(nanoseconds time = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())) -> ProfilerEvent& {
			ProfilerEvent::time = time;
			ProfilerEvent::name[0] = '\0';
			
			return *this;
		}
		
//		friend std::ostream& operator<<(std::ostream &strm, const ProfilerEvent &event) {
//			return strm << event.time << " " << event.name;
//		}
};
class ProfilerEvents {
	public:

		void start(nanoseconds time, std::string name);
		void start(const char* name);
		void start(std::string name);
		void end(nanoseconds time = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()));

		void clear();

	private:
		std::vector<ProfilerEvent> events;
};

#endif /* SRC_PROFILING_HPP_ */
