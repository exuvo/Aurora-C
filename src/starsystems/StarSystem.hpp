/*
 * StarSystem.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_STARSYSTEM_HPP_
#define SRC_STARSYSTEMS_STARSYSTEM_HPP_

#include <chrono>

#include "entt/entt.hpp"
#include "log4cxx/logger.h"

using namespace std::chrono;
using namespace log4cxx;

class StarSystem {
	public:
		nanoseconds updateTime;
		float updateTimeAverage;

		void update(int deltaGameTime);

	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystem");
};



#endif /* SRC_STARSYSTEMS_STARSYSTEM_HPP_ */
