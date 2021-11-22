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
		nanoseconds time;
		char name[50]; // Set to start event, empty to end previous
		
		ProfilerEvent(): time(0ns) {};
		
		// Start
		ProfilerEvent(const char* name): ProfilerEvent(getNanos(), name) {};
		ProfilerEvent(std::string name): ProfilerEvent(getNanos(), name) {};
		ProfilerEvent(nanoseconds time, const char* name_in): time(time) {
			char* w = name;
			while(*name_in != '\0' && (size_t)(w - name) < ARRAY_LEN(name) - 1UL) {
				*w++ = *name_in++;
			}
			*w = '\0';
		};
		ProfilerEvent(nanoseconds time, std::string name_in): time(time) {
			name_in.copy(name, ARRAY_LEN(name));
		};
		ProfilerEvent(const char* name_in, size_t length): time(getNanos()) {
			size_t len = std::min(ARRAY_LEN(name) - 1, length);
			memcpy(name, name_in, len);
			name[len] = '\0';
		};
		
		// End
		ProfilerEvent(nanoseconds time = getNanos()): time(time) {
			name[0] = '\0';
		};
		
//		auto start(const char* name) -> ProfilerEvent& {
//			return start(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()), name);
//		}
//		
//		auto start(std::string name) -> ProfilerEvent& {
//			return start(duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()), name);
//		}
//		
//		auto start(nanoseconds time, const char* name_in) -> ProfilerEvent& {
//			ProfilerEvent::time = time;
//			strncpy(name, name_in, ARRAY_LEN(name));
//			
//			return *this;
//		}
//		
//		auto start(nanoseconds time, std::string name_in) -> ProfilerEvent& {
//			ProfilerEvent::time = time;
//			name_in.copy(name, ARRAY_LEN(name));
//			
//			return *this;
//		}
//		
//		auto end(nanoseconds time = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch())) -> ProfilerEvent& {
//			ProfilerEvent::time = time;
//			name[0] = '\0';
//			
//			return *this;
//		}
		
//		friend std::ostream& operator<<(std::ostream &strm, const ProfilerEvent &event) {
//			return strm << event.time << " " << event.name;
//		}
};
class ProfilerEvents {
	public:

		void start(nanoseconds time, std::string name);
		void start(const char* name);
		void start(const char* name, size_t length);
		void start(std::string name);
		void end(nanoseconds time = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()));
		
		const ProfilerEvent& operator[](size_t idx) const;

		size_t size() const;
		void clear();

	private:
		std::vector<ProfilerEvent> events;
};

#endif /* SRC_PROFILING_HPP_ */
