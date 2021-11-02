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
#include <unordered_map>

#include "entt/entt.hpp"
#include "log4cxx/logger.h"

#include "starsystems/systems/Scheduler.hpp"
#include "starsystems/components/IDComponents.hpp"
#include "utils/Profiling.hpp"

using namespace std::chrono;
using namespace log4cxx;

class Command;
class StarSystem;
class Galaxy;
class ShadowStarSystem;
class Systems;
class Empire;

class StarSystem {
	public:
		static thread_local StarSystem* current; // thread_local has access penalty https://stackoverflow.com/questions/13106049/what-is-the-performance-penalty-of-c11-thread-local-variables-in-gcc-4-8
	
		std::string name;
		entt::entity galacticEntityID = entt::null;
		nanoseconds updateTime = 0ns;
		float updateTimeAverage = 0.0f;
		
		boost::circular_buffer<Command*> commandQueue {128};
		ShadowStarSystem* shadow = nullptr;
		ShadowStarSystem* workingShadow = nullptr;
		bool skipClearShadowChanged = false;
		
		Galaxy* galaxy = nullptr;
		entt::registry registry;
		Systems* systems = nullptr;
		
		StarSystem(std::string name) {
			this->name = name;
		}
		StarSystem(const StarSystem&) = default;
		StarSystem(StarSystem&&) = default;
		
		entt::entity createEnttiy(Empire& empire);
		void destroyEntity(entt::entity entity);
		
		void init(Galaxy* galaxy);
		void update(uint32_t deltaGameTime);
		Scheduler<std::uint32_t> scheduler;
		
		template<typename Component>
		void added(entt::registry&, entt::entity);
		
		template<typename Component>
		void changed2(entt::entity entity);
		
		template<typename... Component>
		void changed(entt::entity entity) {
			(changed2<Component>(entity), ...);
		}
		
		template<typename Component>
		void deleted(entt::registry&, entt::entity);
		
		std::unordered_map<EntityUUID, entt::entity, EntityUUID::hasher> uuids;
		EntityReference getEntityReference(entt::entity entity);
		
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystem");
		uint32_t entityUIDCounter = 0;
};

std::ostream& operator<<(std::ostream& os, const StarSystem& s);

#endif /* SRC_STARSYSTEMS_STARSYSTEM_HPP_ */
