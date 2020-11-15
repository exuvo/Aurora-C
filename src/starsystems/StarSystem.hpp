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

using namespace std::chrono;
using namespace log4cxx;

class Command;
class StarSystem;

class ShadowStarSystem {
		public:
		
		ShadowStarSystem(StarSystem* starSystem) {
			this->starSystem = starSystem;
		}
		
		void update();
		
	private:
		StarSystem* starSystem;
		entt::registry registry;
		std::vector<bool> added;
		std::vector<bool> changed;
		std::vector<bool> deleted;
};

class StarSystem {
	public:
		std::string name;
		entt::id_type galacticEntityID = entt::null;
		nanoseconds updateTime = 0ns;
		float updateTimeAverage = 0.0f;
		
		boost::circular_buffer<Command*> commandQueue {128};
		ShadowStarSystem* shadow = new ShadowStarSystem(this);
		ShadowStarSystem* workingShadow = new ShadowStarSystem(this);

		StarSystem(std::string name) {
			this->name = name;
		}
		StarSystem(const StarSystem&) = default;
		StarSystem(StarSystem&&) noexcept = default;
		
		void init();
		void update(uint32_t deltaGameTime);

	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystem");
		entt::registry registry;
		entt::scheduler<std::uint32_t> scheduler;
		
};



#endif /* SRC_STARSYSTEMS_STARSYSTEM_HPP_ */
