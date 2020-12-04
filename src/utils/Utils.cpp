/*
 * Utils.cpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#include <cstring>
#include <iostream>
#include <exception>

#if defined _WIN32
#include <processthreadsapi>
#elif defined __CYGWIN__
#define THREAD_PRIORITY_BELOW_NORMAL -1
#define THREAD_PRIORITY_NORMAL 0
#define THREAD_PRIORITY_ABOVE_NORMAL 1
#define THREAD_PRIORITY_HIGHEST 2
#include <w32api/processthreadsapi.h>
#include <w32api/errhandlingapi.h>
#elif defined __unix__
#include <pthread.h>
#endif

#include "log4cxx/logger.h"

#include "utils/Utils.hpp"

using namespace log4cxx;

static LoggerPtr log = Logger::getLogger("aurora.utils");

void setThreadPriority(std::thread &thread, ThreadPriority prio) {
	
	std::thread::native_handle_type tHandle = thread.native_handle();
	
#if (defined _WIN32) // or (defined __CYGWIN__)
	
	int priority;
	
	if (prio == ThreadPriority::LOW) {
		priority = THREAD_PRIORITY_BELOW_NORMAL;
	} else if (prio == ThreadPriority::NORMAL) {
		priority = THREAD_PRIORITY_NORMAL;
	} else if (prio == ThreadPriority::HIGH) {
		priority = THREAD_PRIORITY_ABOVE_NORMAL;
	} else if (prio == ThreadPriority::HIGHER) {
		priority = THREAD_PRIORITY_HIGHEST;
	}
	
	int err = SetThreadPriority(tHandle, priority);
	
	if (err) {
		int err2 = GetLastError();
		LOG4CXX_WARN(log, "Failed to set thread priority: " << err << ", " << err2);
	}
	
#elif defined __unix__
	
	// Not working correctly, spamming the log with warnings

	/*
	sched_param sch;
	int policy;
	pthread_getschedparam(tHandle, &policy, &sch);
	
	if (prio == ThreadPriority::LOW) {
		sch.sched_priority = 18;
	} else if (prio == ThreadPriority::NORMAL) {
		sch.sched_priority = 19;
	} else if (prio == ThreadPriority::HIGH) {
		sch.sched_priority = 20;
	} else if (prio == ThreadPriority::HIGHER) {
		sch.sched_priority = 21;
	} else {
		throw std::logic_error("Invalid prio parameter");
	}
	
	int err = pthread_setschedparam(tHandle, SCHED_FIFO, &sch);
	
	if (err) {
		std::ostringstream out;
		out << "Failed to set thread priority: ";
		
		if (err == ESRCH) {
			out << "ESRCH No thread with the ID " << tHandle << " could be found.";
		} else if (err == EINVAL) {
			out << "EINVAL Policy is not a recognized policy, or param does not make sense for the policy.";
		} else if (err == EPERM) {
			out << "EPERM The caller does not have appropriate privileges to set the specified scheduling policy and parameters.";
		} else if (err == ENOTSUP) {
			out << "ENOTSUP Attempt was made to set the policy or scheduling parameters to an unsupported value";
		} else {
			out << err;
		}
		
		if (errno) {
			out << ", errno: " << std::strerror(errno);
		}
		
		LOG4CXX_WARN(log, out.str());
	}*/
#endif
}

namespace entt {
	std::ostream& operator<<(std::ostream& os, const entt::entity& e) {
		return os << (uint32_t) entt::registry::entity(e) << ":" << (uint32_t) entt::registry::version(e);
	}
}
