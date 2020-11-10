/*
 * Galaxy.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_GALAXY_HPP_
#define SRC_GALAXY_GALAXY_HPP_

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "entt/entt.hpp"
#include "log4cxx/logger.h"

#include "starsystems/StarSystem.hpp"

using namespace std::chrono;
using namespace log4cxx;

class Galaxy {
	public:
		std::vector<StarSystem> systems;
		std::thread* galaxyThread = nullptr;
		bool shutdown = false;
		nanoseconds speed = 1s;
		int ticksize = 1;

		Galaxy(std::vector<StarSystem>& systems) {
			Galaxy::systems = std::move(systems);
		}
		void init();

	private:
		LoggerPtr log = Logger::getLogger("aurora.galaxy");
		std::vector<std::thread*> threads;
		std::mutex mutex;
		std::condition_variable condvar;
		std::atomic<uint32_t> takenWorkCounter;
		std::atomic<uint32_t> completedWorkCounter;

		void galaxyWorker();
		void starsystemWorker();
};

#endif /* SRC_GALAXY_GALAXY_HPP_ */
