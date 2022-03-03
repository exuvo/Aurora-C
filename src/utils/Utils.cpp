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
#include <fmt/core.h>

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
#if (defined __GNUC__ || defined __clang__) 
#include <execinfo.h>
#endif
#endif

#include <log4cxx/logger.h>

#include "utils/Utils.hpp"
#include "utils/Math.hpp"

using namespace log4cxx;

static LoggerPtr utilLog = Logger::getLogger("aurora.utils");

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
				
				LOG4CXX_WARN(utilLog, out.str());
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
				LOG4CXX_WARN(utilLog, "soft rlimit of " << niceLimits.rlim_cur << " restricts lowering niceness to desired value " << niceness << ". Increase limit in /etc/security/limits.conf to 20. Message won't be printed again.");
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
			
			LOG4CXX_WARN(utilLog, out.str());
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
			
			LOG4CXX_WARN(utilLog, out.str());
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

using Units::NANO_MICRO;
using Units::NANO_MILLI;
using Units::NANO_SECOND;

//std::string nanoToString(uint64_t time) {
//	uint32_t nanos = time % NANO_MILLI;
//	uint32_t milli = time / NANO_MILLI;
//	
//	return fmt::format("{}.{:06}ms", milli, nanos);
//}
//
//std::string nanoToMicroString(uint64_t time) {
//	uint32_t nanos = time % NANO_MICRO;
//	uint32_t micro = time / NANO_MICRO;
//	
//	return fmt::format("{}.{:03}us", micro, nanos);
//}

std::string milliToString(uint64_t time) {
	uint32_t micros = time % 1000;
	uint32_t seconds = time / 1000;
	
	return fmt::format("{}.{:06}s", seconds, micros);
}

std::string secondsToString(uint64_t time) {
	uint32_t seconds = time % 60;
	uint32_t minutes = (time / 60) % 60;
	uint32_t hours = (time / (60 * 60)) % 24;
	
	return fmt::format("{:02}:{:02}:{:02}", hours, minutes, seconds);
}

using Units::KILO;
using Units::MEGA;
using Units::GIGA;
using Units::TERA;
using Units::CUBIC_DECIMETRE;
using Units::CUBIC_METRE;


std::string powerToString(uint64_t power) {
	if (power < KILO) {
		return fmt::format("{} W", power);
		
	} else if (power < MEGA) {
		return fmt::format("{}.{:02} kW", power / KILO, power % KILO / 10);
		
	} else if (power < GIGA) {
		return fmt::format("{}.{:02} MW", power / MEGA, power % MEGA / 10 / KILO);
		
	} else if (power < TERA) {
		return fmt::format("{}.{:02} GW", power / GIGA, power % GIGA / 10 / MEGA);
		
	} else if (power < 1000 * TERA) {
		return fmt::format("{}.{:02} TW", power / TERA, power % TERA / 10 / GIGA);
		
	} else {
		return fmt::format("{} TW", power / TERA);
	}
}

std::string capacityToString(uint64_t capacity) {
	if (capacity < KILO) {
		return fmt::format("{} J", capacity);
		
	} else if (capacity < MEGA) {
		return fmt::format("{}.{:02} kJ", capacity / KILO, capacity % KILO / 10);
		
	} else if (capacity < GIGA) {
		return fmt::format("{}.{:02} MJ", capacity / MEGA, capacity % MEGA / 10 / KILO);
		
	} else if (capacity < TERA) {
		return fmt::format("{}.{:02} GJ", capacity / GIGA, capacity % GIGA / 10 / MEGA);
		
	} else if (capacity < 1000 * TERA) {
		return fmt::format("{}.{:02} TJ", capacity / TERA, capacity % TERA / 10 / GIGA);
		
	} else {
		return fmt::format("{} TJ", capacity / TERA);
	}
}

std::string volumeToString(uint64_t volume) {
	if (volume < CUBIC_DECIMETRE) {
		return fmt::format("{} cm³", volume);
		
	} else if (volume < CUBIC_METRE) {
		return fmt::format("{}.{:02} dm³", volume / CUBIC_DECIMETRE, volume % CUBIC_DECIMETRE / 10);
		
	} else {
		return fmt::format("{} m³", volume / CUBIC_METRE);
	}
}

std::string massToString(uint64_t mass) {
	if (mass < KILO) {
		return fmt::format("{} kg", mass);
		
	} else if (mass < MEGA) {
		return fmt::format("{}.{:02} Mg", mass / KILO, mass % KILO / 10);
		
	} else if (mass < GIGA) {
		return fmt::format("{}.{:02} Gg", mass / MEGA, mass % MEGA / 10 / KILO);
		
	} else if (mass < TERA) {
		return fmt::format("{}.{:02} Tg", mass / GIGA, mass % GIGA / 10 / MEGA);
		
	} else if (mass < 1000 * TERA) {
		return fmt::format("{}.{:02} Pg", mass / TERA, mass % TERA / 10 / GIGA);
		
	} else {
		return fmt::format("{} Pg", mass / TERA);
	}
}

std::string distanceToString(uint64_t distance) {
	if (distance < KILO) {
		return fmt::format("{} m", distance);
		
	} else if (distance < MEGA) {
		return fmt::format("{}.{:02} km", distance / KILO, distance % KILO / 10);
		
	} else if (distance < GIGA) {
		return fmt::format("{}.{:02} Mm", distance / MEGA, distance % MEGA / 10 / KILO);
		
	} else if (distance < TERA) {
		return fmt::format("{}.{:02} Gm", distance / GIGA, distance % GIGA / 10 / MEGA);
		
	} else if (distance < 1000 * TERA) {
		return fmt::format("{}.{:02} Tm", distance / TERA, distance % TERA / 10 / GIGA);
		
	} else {
		return fmt::format("{} Pm", distance / TERA);
	}
}

std::string daysToRemaining(uint32_t days) {
	if (days <= 365) {
		
		return fmt::format("{:3} days", days);
		
	} else {
		
		int year = days / 365;
		
		return fmt::format("{} years {:3} days", year, days % 365);
	}
}

std::string daysToDate(uint32_t days) {
	int year = 2100 + days / 365;
	
	return fmt::format("{:04}-{:03}", year, 1 + days % 365);
}

std::string daysToYear(uint32_t days) {
	int year = 2100 + days / 365;
	
	return fmt::format("{:04}", year);
}

std::string daysToSubYear(uint32_t days) {
	int year = days / 365;
	
	return fmt::format("{:02}", year % 100);
}

std::string demangleTypeName(std::string tname) {
#if defined(__clang__) || defined(__GNUG__)
	int status;
	char* demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
	if (status == 0) {
		tname = demangled_name;
		std::free(demangled_name);
	}
#endif
	return {tname};
}

std::string type_name(const void* ptr) {
	std::string tname = typeid(ptr).name();
	return demangleTypeName(tname);
}

// https://stackoverflow.com/questions/11665829/how-can-i-print-stack-trace-for-caught-exceptions-in-c-code-injection-in-c/11674810#11674810
#if not defined NDEBUG && __unix__
#include <dlfcn.h>
#include <boost/stacktrace.hpp>

namespace {
  void* last_frames[20];
  size_t last_size = 0;
  std::string exception_name;
}

extern "C" {
  void __cxa_throw(void *ex, void *info, void (*dest)(void *)) {
    exception_name = demangleTypeName(reinterpret_cast<const std::type_info*>(info)->name());
    last_size = backtrace(last_frames, ARRAY_LEN(last_frames));

    static void (*const rethrow)(void*,void*,void(*)(void*)) __attribute__ ((noreturn)) = (void (*)(void*,void*,void(*)(void*))) dlsym(RTLD_NEXT, "__cxa_throw");
    rethrow(ex, info, dest);
  }
}

std::string getLastExceptionStacktrace() {
//  char** names = backtrace_symbols(last_frames, last_size);
  std::stringstream ss;
  
  for (size_t i=0; i < last_size; i++) {
//  	ss << names[i] << std::endl;
  	ss << boost::stacktrace::to_string(boost::stacktrace::frame(last_frames[i])) << std::endl;
  }
  
//  free(names);
  return ss.str();
}

void printCurrentStacktrace(std::ostream& out) {
	void* frames[20];
	size_t size = backtrace(frames, ARRAY_LEN(frames));
	
	for (size_t i=0; i < size; i++) {
		out << ' ' << i << "# " << boost::stacktrace::to_string(boost::stacktrace::frame(frames[i])) << std::endl;
	}
}

std::string getCurrentStacktrace() {
  std::stringstream ss;
  printCurrentStacktrace(ss);
  return ss.str();
}

// Replacement for glibc libSegFault which was removed in 2.35: https://github.com/jonathanpoelen/libsegfault
// Alternative GNU libsigsegv
#include <csignal>
#include <unistd.h>

__sighandler_t oldSignalHandlerSegv;
__sighandler_t oldSignalHandlerBus;
__sighandler_t oldSignalHandlerIll;
__sighandler_t oldSignalHandlerAbrt;
__sighandler_t oldSignalHandlerFpe;
__sighandler_t oldSignalHandlerSys;

void signal_handler(int signum) {
	if (signum == SIGSEGV) {
		fprintf(stderr, "Segmentation fault, Invalid memory access:\n");
	} else if (signum == SIGBUS) {
		fprintf(stderr, "Bus error:\n");
	} else if (signum == SIGILL) {
		fprintf(stderr, "Illegal instruction:\n");
	} else if (signum == SIGABRT) {
		fprintf(stderr, "Abnormal termination:\n");
	} else if (signum == SIGFPE) {
		fprintf(stderr, "Erroneous arithmetic operation:\n");
	} else if (signum == SIGSYS) {
		fprintf(stderr, "Bad system call:\n");
	} else {
		fprintf(stderr, "Unknown signal %d:\n", signum);
	}
	
	printCurrentStacktrace(std::cerr);
	
//	::signal(signum, SIG_DFL);
//	::raise(signum);
	
	exit(1);
}

#include <sys/resource.h> // Enable core dumps

void setupSignalHandler(void) {
	oldSignalHandlerSegv = signal(SIGSEGV, signal_handler);
	oldSignalHandlerBus = signal(SIGBUS,  signal_handler);
	oldSignalHandlerIll = signal(SIGILL,  signal_handler);
	oldSignalHandlerAbrt = signal(SIGABRT, signal_handler);
	oldSignalHandlerFpe = signal(SIGFPE,  signal_handler);
	oldSignalHandlerSys = signal(SIGSYS,  signal_handler);
	
	// Enable core dumps
//	struct rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
//	setrlimit(RLIMIT_CORE, &core_limit);
}

#include <sys/wait.h>
#include <sys/prctl.h>

// Can not be called from a signal context
void printCurrentStacktraceGDB() {
	char pid_buf[30];
	sprintf(pid_buf, "%d", getpid());
	
	char name_buf[256];
	name_buf[readlink("/proc/self/exe", name_buf, 255)] = 0;
	
	prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
	int child_pid = fork();
	if (!child_pid) {
		auto dummyHandler = [](int signum){
			fprintf(stderr, "dummy signal handler: %d\n", signum);
			exit(1);
		};
		
		signal(SIGSEGV, dummyHandler);
		signal(SIGBUS,  dummyHandler);
		signal(SIGILL,  dummyHandler);
		signal(SIGABRT, dummyHandler);
		signal(SIGFPE,  dummyHandler);
		signal(SIGSYS,  dummyHandler);
		
		fprintf(stderr, "starting gdb\n");
		dup2(2, 1); // redirect output to stderr
		execl("/usr/bin/gdb", "gdb", "--batch", "-n", "-ex", "thread", "-ex", "bt", name_buf, pid_buf, NULL);
		fprintf(stderr, "aborting\n");
		abort(); /* If gdb failed to start */
	} else {
		waitpid(child_pid, NULL, 0);
	}
}

#else
std::string getLastExceptionStacktrace() {
	return {};
}
int setupSignalHandler(void) {
	return 0;
}
#endif
