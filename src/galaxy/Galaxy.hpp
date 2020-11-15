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
#include "galaxy/Empire.hpp"
#include "galaxy/Player.hpp"
#include "utils/Profiling.hpp"

using namespace std::chrono;
using namespace log4cxx;

class Galaxy;

class ShadowGalaxy {
	public:
		entt::registry registry;
		std::vector<bool> added;
		std::vector<bool> changed;
		std::vector<std::vector<bool>> changedComponents;
		std::vector<bool> deleted;
		
		ProfilerEvents profilerEvents;
		
		ShadowGalaxy(Galaxy* galaxy) {
			this->galaxy = galaxy;
//			changedComponents.reserve(registry.);
		}
		
		void update();
		
	private:
		Galaxy* galaxy;
};

class Galaxy {
	public:
		std::vector<StarSystem*> systems;
		std::thread* galaxyThread = nullptr;
		bool shutdown = false;
		bool sleeping = false;
		ShadowGalaxy* shadow = new ShadowGalaxy(this);
		std::recursive_mutex shadowLock;
		ProfilerEvents renderProfilerEvents;
		
		seconds time = 0s;
		uint32_t day = updateDay();
		nanoseconds speed = 1s;
		bool speedLimited = false;
		uint32_t tickSize = 1;
		
		std::vector<Empire> empires;
		std::vector<Player> players;
		

		Galaxy(std::vector<Empire>& empires, std::vector<StarSystem*>& systems) {
			Galaxy::empires = std::move(empires);
			Galaxy::systems = std::move(systems);
		}
		Galaxy(const Galaxy&) = default;
		Galaxy(Galaxy&&) = default;
		
		void init();

	private:
		LoggerPtr log = Logger::getLogger("aurora.galaxy");
		std::vector<std::thread*> threads;
		std::mutex mutex;
		std::condition_variable condvar;
		std::atomic<uint32_t> takenWorkCounter;
		std::atomic<uint32_t> completedWorkCounter;
		ShadowGalaxy* workingShadow = new ShadowGalaxy(this);
		
		entt::registry registry;
		entt::scheduler<std::uint32_t> scheduler;

		void galaxyWorker();
		void starsystemWorker();
		int updateDay();
};

#endif /* SRC_GALAXY_GALAXY_HPP_ */
