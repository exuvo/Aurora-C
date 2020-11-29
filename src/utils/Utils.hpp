/*
 * Utils.hpp
 *
 *  Created on: 14 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_UTILS_UTILS_HPP_
#define SRC_UTILS_UTILS_HPP_

#include <thread>

#include "entt/entt.hpp"

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

enum class ThreadPriority {
		LOW,
		NORMAL,
		HIGH,
		HIGHER
};

void setThreadPriority(std::thread& thread, ThreadPriority prio);

namespace entt {
	std::ostream& operator<<(std::ostream& os, const entt::entity& e);
}

#endif /* SRC_UTILS_UTILS_HPP_ */
