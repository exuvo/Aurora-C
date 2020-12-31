/*
 * StarSystem.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_STARSYSTEM_SHADOW_HPP_
#define SRC_STARSYSTEMS_STARSYSTEM_SHADOW_HPP_

#include <boost/hana.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/seq.hpp>

#include "StarSystem.hpp"
#include "components/Components.hpp"
#include "starsystems/systems/Systems.hpp"
#include "utils/quadtree/QuadTreeAABB.hpp"
#include "utils/quadtree/QuadTreePoint.hpp"
#include "utils/BitVector.hpp"

namespace hana = boost::hana;

// https://www.boost.org/doc/libs/1_74_0/libs/preprocessor/doc/AppendixA-AnIntroductiontoPreprocessorMetaprogramming.html
#define SYNCED_COMPONENTS_TUPLE (TextComponent, TintComponent, RenderComponent, TimedMovementComponent, ThrustComponent, SpatialPartitioningComponent, SpatialPartitioningPlanetoidsComponent, UUIDComponent) // max 25, after that write sequence directly (a)(b)(c)
#define SYNCED_COMPONENTS_SEQ BOOST_PP_TUPLE_TO_SEQ(SYNCED_COMPONENTS_TUPLE)
#define SYNCED_COMPONENTS_SEQ_SIZE BOOST_PP_SEQ_SIZE(SYNCED_COMPONENTS_SEQ)
#define SYNCED_COMPONENTS BOOST_PP_SEQ_ENUM(SYNCED_COMPONENTS_SEQ)
#define SYNCED_COMPONENTS_FORWARD_DECLARATIONS(r, unused, component) struct component;

class StarSystem;
BOOST_PP_SEQ_FOR_EACH(SYNCED_COMPONENTS_FORWARD_DECLARATIONS, ~, SYNCED_COMPONENTS_SEQ)

#define SYNCED_COMPONENTS_MAP_MACRO(z, n, unused) hana::make_pair(hana::type_c<BOOST_PP_SEQ_ELEM(n, SYNCED_COMPONENTS_SEQ)>, n)

constexpr auto syncedComponentToIndexMap = hana::make_map(
		BOOST_PP_ENUM(SYNCED_COMPONENTS_SEQ_SIZE, SYNCED_COMPONENTS_MAP_MACRO, ~)
);

class ShadowStarSystem {
	public:
		ShadowStarSystem(StarSystem& starSystem): starSystem(starSystem) {}
		
		StarSystem& starSystem;
		entt::registry registry;
		std::unordered_map<EntityUUID, entt::entity, EntityUUID::hasher> uuids;
		
		BitVector added;
		BitVector changed;
		BitVector changedComponents[SYNCED_COMPONENTS_SEQ_SIZE];
		BitVector deleted;

		bool uuidsChanged = false;
		bool quadtreeShipsChanged = false;
		bool quadtreePlanetoidsChanged = false;
		
		QuadtreePoint quadtreeShips = {SpatialPartitioningSystem::MAX, SpatialPartitioningSystem::MAX, SpatialPartitioningSystem::MAX_ELEMENTS, SpatialPartitioningSystem::DEPTH};
		QuadtreeAABB quadtreePlanetoids = {SpatialPartitioningPlanetoidsSystem::MAX, SpatialPartitioningPlanetoidsSystem::MAX, SpatialPartitioningPlanetoidsSystem::MAX_ELEMENTS, SpatialPartitioningPlanetoidsSystem::DEPTH};

		ProfilerEvents profilerEvents;

		void update();
		EntityReference getEntityReference(entt::entity entity);

	private:
		BitVector tmp;
		BitVector tmpComponents[SYNCED_COMPONENTS_SEQ_SIZE];
		
		void addComponents(uint32_t entityID, entt::entity entity);
		void updateComponents(uint32_t entityID, entt::entity entity);
};

#endif /* SRC_STARSYSTEMS_STARSYSTEM_SHADOW_HPP_ */
