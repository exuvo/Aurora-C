/*
 * Utils.cpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#include <cstring>
#include <iostream>
#include <exception>
#include <algorithm>

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
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
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
	
	static int initialized = false;
	static int minPrio;
	static int maxPrio;
	static struct rlimit niceLimits;
	
	if (!initialized) {
		sched_param sch;
		int policy;
		pthread_getschedparam(tHandle, &policy, &sch);
		
		minPrio = sched_get_priority_min(policy);
		maxPrio = sched_get_priority_max(policy);
		getrlimit(RLIMIT_NICE, &niceLimits);
		
		if (niceLimits.rlim_cur < 20 && niceLimits.rlim_cur < niceLimits.rlim_max) {
			niceLimits.rlim_cur = std::min(niceLimits.rlim_max, 20ul);
			errno = 0;
			int err = setrlimit(RLIMIT_NICE, &niceLimits);
			if (err) {
				std::ostringstream out;
				out << "Failed increase rlimit to " << niceLimits.rlim_cur;
				
				if (err == EPERM) {
					out << "EPERM The process tried to raise a current limit beyond the maximum limit. ";
				} else {
					out << "err " << err;
				}
				
				if (errno) {
					out << ", errno: " << std::strerror(errno);
				}
				
				LOG4CXX_WARN(log, out.str());
			}
			
			getrlimit(RLIMIT_NICE, &niceLimits);
		}
		
		initialized = true;
	}
	
	int err;
	
	if (minPrio == maxPrio) { // In linux normal threads (SCHED_OTHER) can only change nice value not priority
		
		int niceness;
	
		if (prio == ThreadPriority::LOW) {
			niceness = 1;
		} else if (prio == ThreadPriority::NORMAL) {
			niceness = 0;
		} else if (prio == ThreadPriority::HIGH) {
			niceness = 0; //-1;
		} else if (prio == ThreadPriority::HIGHER) {
			niceness = 0; //-2;
		} else {
			throw std::logic_error("Invalid prio parameter");
		}
		
		pid_t tid = syscall(__NR_gettid);
		
		int currentNiceness = getpriority(PRIO_PROCESS, tid);
		
		int softNiceLimit = 20 - niceLimits.rlim_cur;
		if (niceness < currentNiceness && niceness < softNiceLimit) { // Can not lower niceness
			static bool once = false;
			if (!once) {
				once = true;
				LOG4CXX_WARN(log, "soft rlimit of " << niceLimits.rlim_cur << " restricts lowering niceness to desired value " << niceness << ". Increase limit in /etc/security/limits.conf to 20. Message won't be printed again.");
			}
			niceness = std::min(currentNiceness, softNiceLimit);
		}
		
		errno = 0;
		int err = setpriority(PRIO_PROCESS, tid, niceness); // On linux PRIO_PROCESS is actually per thread if thread id is sent in
		
		if (err) {
			std::ostringstream out;
			out << "Failed to set thread " << tid << " niceness " << prio << ", current " << currentNiceness << ": ";
			
			if (err == ESRCH) {
				out << "ESRCH No process was located using the which and who values specified.";
			} else if (err == EINVAL) {
				out << "EINVAL which parameter was not one of PRIO_PROCESS, PRIO_PGRP, or PRIO_USER.";
			} else if (err == EPERM) {
				out << "EPERM A process was located, but its effective user ID did not match either the effective or the real user ID of the caller, and was not privileged (on Linux: did not have the CAP_SYS_NICE capability).";
			} else if (err == EACCES) {
				out << "EACCES The caller attempted to lower a process priority, but did not have the required privilege (on Linux: did not have the CAP_SYS_NICE capability). Since Linux 2.6.12, this error only occurs if the caller attempts to set a process priority outside the range of the RLIMIT_NICE soft resource limit of the target process; see getrlimit(2) for details.";
			} else if (err != -1) {
				out << "err " << err;
			}
			
			if (errno) {
				out << " errno: " << std::strerror(errno);
			}
			
			LOG4CXX_WARN(log, out.str());
		}
		
	} else {
		
		int priority;
	
		if (prio == ThreadPriority::LOW) {
			priority = minPrio;
		} else if (prio == ThreadPriority::NORMAL) {
			priority = maxPrio - minPrio;
		} else if (prio == ThreadPriority::HIGH) {
			priority = maxPrio - minPrio + 1;
		} else if (prio == ThreadPriority::HIGHER) {
			priority = maxPrio - minPrio + 2;
		} else {
			throw std::logic_error("Invalid prio parameter");
		}
		
		if (priority > maxPrio) {
			priority = maxPrio;
		}
		
		int err = pthread_setschedprio(tHandle, priority);
		
		if (err) {
			std::ostringstream out;
			out << "Failed to set thread " << tHandle << " priority " << prio;
			
			sched_param sch;
			int policy;
			pthread_getschedparam(tHandle, &policy, &sch);
			
			int minPrio = sched_get_priority_min(policy);
			int maxPrio = sched_get_priority_max(policy);
			
			out << ", current: policy " << policy << ", priority " << sch.sched_priority << ", min prio " << minPrio << ", maxPrio " << maxPrio << ": ";
			
			if (err == ESRCH) {
				out << "ESRCH No thread with the handle " << tHandle << " could be found.";
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
	}
	
#endif
}

namespace entt {
	std::ostream& operator<<(std::ostream& os, const entt::entity& e) {
		return os << (uint32_t) entt::registry::entity(e) << ":" << (uint32_t) entt::registry::version(e);
	}
}

nanoseconds getNanos() {
	return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
}

milliseconds getMillis() {
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch());
}
