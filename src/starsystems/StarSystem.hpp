/*
 * StarSystem.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_STARSYSTEM_HPP_
#define SRC_STARSYSTEMS_STARSYSTEM_HPP_

#include <chrono>
#include <boost/circular_buffer.hpp>

#include "entt/entt.hpp"
#include "log4cxx/logger.h"

#include "starsystems/systems/Scheduler.hpp"
#include "utils/Profiling.hpp"

using namespace std::chrono;
using namespace log4cxx;

class Command;
class StarSystem;
class Galaxy;

class ShadowStarSystem {
		public:
			entt::registry registry;
			std::vector<bool> added;
			std::vector<bool> changed;
			std::vector<std::vector<bool>> changedComponents;
			std::vector<bool> deleted;
			
			bool quadtreeShipsChanged = false;
			bool quadtreePlanetoidsChanged = false;
			
			ProfilerEvents profilerEvents;
		
		ShadowStarSystem(StarSystem* starSystem) {
			this->starSystem = starSystem;
		}
		
		void update();
		
	private:
		StarSystem* starSystem;
};

class StarSystem {
	public:
		std::string name;
		entt::id_type galacticEntityID = entt::null;
		nanoseconds updateTime = 0ns;
		float updateTimeAverage = 0.0f;
		
		boost::circular_buffer<Command*> commandQueue {128};
		ShadowStarSystem* shadow = nullptr;
		ShadowStarSystem* workingShadow = nullptr;
		bool skipClearShadowChanged = false;
		
		Galaxy* galaxy = nullptr;
		entt::registry registry;

		StarSystem(std::string name) {
			this->name = name;
		}
		StarSystem(const StarSystem&) = default;
		StarSystem(StarSystem&&) = default;
		
		void init(Galaxy* galaxy);
		void update(uint32_t deltaGameTime);
		Scheduler<std::uint32_t> scheduler;
		
		template<typename... Component>
		void added(entt::registry&, entt::entity);
		
		template<typename... Component>
		void changed(entt::entity) {
			
		}
		
		template<typename... Component>
		void deleted(entt::registry&, entt::entity);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystem");
};

#endif /* SRC_STARSYSTEMS_STARSYSTEM_HPP_ */
