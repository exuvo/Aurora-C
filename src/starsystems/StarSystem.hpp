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
#include <boost/hana.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/seq.hpp>

#include "entt/entt.hpp"
#include "log4cxx/logger.h"

#include "starsystems/systems/Scheduler.hpp"
#include "components/Components.hpp"
#include "utils/Profiling.hpp"

namespace hana = boost::hana;
using namespace std::chrono;
using namespace log4cxx;

// https://www.boost.org/doc/libs/1_74_0/libs/preprocessor/doc/AppendixA-AnIntroductiontoPreprocessorMetaprogramming.html
#define SYNCED_COMPONENTS_TUPLE (TextComponent, TintComponent, RenderComponent, TimedMovementComponent, ThrustComponent) // max 25, after that write sequence directly (a)(b)(c)
#define SYNCED_COMPONENTS_SEQ BOOST_PP_TUPLE_TO_SEQ(SYNCED_COMPONENTS_TUPLE)
#define SYNCED_COMPONENTS_SEQ_SIZE BOOST_PP_SEQ_SIZE(SYNCED_COMPONENTS_SEQ)
#define SYNCED_COMPONENTS BOOST_PP_SEQ_ENUM(SYNCED_COMPONENTS_SEQ)
#define SYNCED_COMPONENTS_FORWARD_DECLARATIONS(r, unused, component) struct component;

class Command;
class StarSystem;
class Galaxy;
BOOST_PP_SEQ_FOR_EACH(SYNCED_COMPONENTS_FORWARD_DECLARATIONS, ~, SYNCED_COMPONENTS_SEQ)

#define SYNCED_COMPONENTS_MAP_MACRO(z, n, unused) hana::make_pair(hana::type_c<BOOST_PP_SEQ_ELEM(n, SYNCED_COMPONENTS_SEQ)>, n)

constexpr auto syncedComponentToIndexMap = hana::make_map(
		BOOST_PP_ENUM(SYNCED_COMPONENTS_SEQ_SIZE, SYNCED_COMPONENTS_MAP_MACRO, ~)
);

class ShadowStarSystem {
		public:
			entt::registry registry;
			std::vector<bool> added;
			std::vector<bool> changed;
			std::vector<bool> changedComponents[SYNCED_COMPONENTS_SEQ_SIZE];
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
		
		template<typename Component>
		void added(entt::registry&, entt::entity);
		
		template<typename Component>
		void changed2(entt::entity entity) {
			LOG4CXX_DEBUG(log, "starsystem " << name << " changed component " << type_name<Component>() << " of entity " << entity);
			BOOST_HANA_CONSTANT_ASSERT_MSG(hana::find(syncedComponentToIndexMap, hana::type_c<Component>) != hana::nothing, "missing component mapping");
			auto& changedVector = workingShadow->changedComponents[syncedComponentToIndexMap[hana::type_c<Component>]];
			uint32_t index = static_cast<uint32_t>(registry.entity(entity));
			changedVector.reserve(index + 1);
			changedVector[index] = true;
		}
		
		template<typename... Component>
		void changed(entt::entity entity) {
//			(changed2<Component>(entity), ...);
			using expand = int[];
			(void) expand { 0, ((void) changed2<Component>(entity), 0) ... };
		}
		
		template<typename Component>
		void deleted(entt::registry&, entt::entity);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystem");
};

#endif /* SRC_STARSYSTEMS_STARSYSTEM_HPP_ */
