/*
 * Utils.cpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#include <cstring>
#include <iostream>
#include <exception>

#if defined __unix__
#include <pthread.h>
#elif defined _WIN32
#include <processthreadsapi>
#endif

#include "log4cxx/logger.h"

#include "utils/Utils.hpp"

using namespace log4cxx;

static LoggerPtr log = Logger::getLogger("aurora.utils");

void setThreadPriority(std::thread &thread, ThreadPriority prio) {
#ifdef __CYGWIN__
	return; //TODO fixme: giving ENOTSUP for some reason
#endif
	
	std::thread::native_handle_type tHandle = thread.native_handle();
#if defined __unix__
	sched_param sch;
	int policy;
	pthread_getschedparam(tHandle, &policy, &sch);
	
	if (prio == ThreadPriority::LOW) {
		sch.sched_priority = 19;
	} else if (prio == ThreadPriority::NORMAL) {
		sch.sched_priority = 20;
	} else if (prio == ThreadPriority::HIGH) {
		sch.sched_priority = 21;
	} else if (prio == ThreadPriority::HIGHER) {
		sch.sched_priority = 22;
	} else {
		throw std::logic_error("Invalid prio parameter");
	}
	
	int err = pthread_setschedparam(tHandle, SCHED_RR, &sch);
	
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
	}
#elif defined _WIN32
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
	
	SetThreadPriority(tHandle, priority);
#endif
}