/*
 * StarSystemShadow.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include <fmt/core.h>
#include <fmt/format.h>
#include <starsystems/ShadowStarSystem.hpp>

#include "starsystems/components/Components.hpp"
#include "starsystems/StarSystem.hpp"

#undef PROFILE
#undef PROFILE_End
#define PROFILE(x) if (profiling) profilerEvents.start((x));
#define PROFILE2(x, ...) \
if (profiling) { \
	strBuf.clear(); \
	fmt::format_to(std::back_inserter(strBuf), (x), ##__VA_ARGS__); \
	profilerEvents.start(strBuf.data(), strBuf.size()); \
};
#define PROFILE_End() if (profiling) profilerEvents.end();

static entt::entity getCurrentEntity(entt::registry& registry, uint32_t entityID) {
	entt::entity entity = static_cast<entt::entity>(entityID);
	uint32_t currentVersion = registry.current(entity);
	return static_cast<entt::entity>(entityID | (currentVersion << entt::entt_traits<uint32_t>::entity_shift));
};

static entt::entity getEntity(uint32_t entityID) { // No version number
	return static_cast<entt::entity>(entityID);;
};

void ShadowStarSystem::update() {
	ShadowStarSystem* otherShadow = starSystem.shadow;
	
	{
		uint32_t size = std::max({added.size(), changed.size(), deleted.size(), otherShadow->added.size(), otherShadow->changed.size(), otherShadow->deleted.size()});
		tmp.reserve(size);
		added.reserve(size);
		changed.reserve(size);
		deleted.reserve(size);
		
		for (uint32_t i=0; i < SYNCED_COMPONENTS_SEQ_SIZE; i++) {
			tmpComponents[i].reserve(size);
		}
	}
	
	auto strBuf = fmt::memory_buffer();
	
	// Skip added and deleted in same tick
	tmp = added;
	tmp &= deleted;
	added %= tmp;
	changed %= tmp;
	deleted %= tmp;
	
	changed %= added; // Skip created and modified in same tick
	
	PROFILE("deleted");
	tmp = deleted;
	tmp |= otherShadow->deleted;
	for (auto entityID : tmp) {
		entt::entity entity = getCurrentEntity(registry, entityID);
		registry.destroy(entity);
	}
	PROFILE_End();
	
	tmp = changed;
	tmp |= otherShadow->changed;
	tmp %= otherShadow->added; // Skip other added as they will be handled as added below
	tmp %= deleted; // Skip now deleted from other changed
	tmp %= otherShadow->deleted; // Skip other deleted from other changed
	
	for (uint32_t i=0; i < SYNCED_COMPONENTS_SEQ_SIZE; i++) {
		tmpComponents[i] = changedComponents[i];
		tmpComponents[i] |= otherShadow->changedComponents[i];
	}
	
	PROFILE("changed");
	for (auto entityID : tmp) {
		PROFILE2("{}", entityID);
		entt::entity entity = getCurrentEntity(registry, entityID);
		updateComponents(entityID, entity);
		PROFILE_End();
	}
	PROFILE_End();
	
	tmp = added;
	tmp |= otherShadow->added;
	tmp %= deleted; // Skip other added that are now deleted
	
	PROFILE("added");
	for (auto entityID : tmp) {
		PROFILE2("{}", entityID);
		entt::entity entity = registry.create(getEntity(entityID));
		assert(entityID == static_cast<uint32_t>(registry.entity(entity)));
		addComponents(entityID, entity);
		PROFILE_End();
	}
	PROFILE_End();
	
	if (quadtreeShipsChanged || starSystem.shadow->quadtreeShipsChanged) {
		PROFILE("copy quadtree ships");
		quadtreeShips = starSystem.systems->spatialPartitioningSystem->tree;
		PROFILE_End();
	}
	
	if (quadtreePlanetoidsChanged || starSystem.shadow->quadtreePlanetoidsChanged) {
		PROFILE("copy quadtree planetoids");
		quadtreePlanetoids = starSystem.systems->spatialPartitioningPlanetoidsSystem->tree;
		PROFILE_End();
	}
	
	if (uuidsChanged || starSystem.shadow->uuidsChanged) {
		PROFILE("copy uuids map");
		uuids = starSystem.uuids;
		PROFILE_End();
	}
}

EntityReference ShadowStarSystem::getEntityReference(entt::entity entity) {
	return {&starSystem, entity, registry.get<UUIDComponent>(entity).uuid};
}

#define ADD_TEMPLATE(r, unused, component) \
{ \
	component* realComp = starSystem.registry.try_get<component>(realEntity); \
	if (realComp) { \
		component& shadowComp = registry.emplace<component>(shadowEntity); \
		shadowComp = *realComp; \
	} \
}

void ShadowStarSystem::addComponents(uint32_t entityID, entt::entity shadowEntity) {
	entt::entity realEntity = getCurrentEntity(starSystem.registry, entityID);
	
//	{ // debug
//		TimedMovementComponent* realComp = starSystem.registry.try_get<TimedMovementComponent>(realEntity);
//		if (realComp) {
//			TimedMovementComponent& shadowComp = registry.emplace<TimedMovementComponent>(shadowEntity);
//			shadowComp = *realComp;
//			registry.remove<TimedMovementComponent>(shadowEntity);
//		}
//	}
	
	BOOST_PP_SEQ_FOR_EACH(ADD_TEMPLATE, ~, SYNCED_COMPONENTS_SEQ);
}

#define UPDATE_TEMPLATE(r, unused, component) \
if (tmpComponents[syncedComponentToIndexMap[hana::type_c<component>]][entityID]) { \
	component* realComp = starSystem.registry.try_get<component>(realEntity); \
	if (realComp) { \
		component* shadowComp; \
		if (registry.all_of<component>(shadowEntity)) { \
			shadowComp = &registry.get<component>(shadowEntity); \
		} else { \
			shadowComp = &registry.emplace<component>(shadowEntity); \
		} \
		*shadowComp = *realComp; \
	} else { \
		registry.remove_if_exists<component>(shadowEntity); \
	} \
}

void ShadowStarSystem::updateComponents(uint32_t entityID, entt::entity shadowEntity) {
	entt::entity realEntity = getCurrentEntity(starSystem.registry, entityID);
	TimedMovementComponent* realComp = starSystem.registry.try_get<TimedMovementComponent>(realEntity);
	if (realComp) {
		TimedMovementComponent* shadowComp;
		if (registry.all_of<TimedMovementComponent>(shadowEntity)) {
			shadowComp = &registry.get<TimedMovementComponent>(shadowEntity);
		} else {
			shadowComp = &registry.emplace<TimedMovementComponent>(shadowEntity);
		}
		*shadowComp = *realComp;
	} else {
		registry.remove_if_exists<TimedMovementComponent>(shadowEntity);
	} 
	BOOST_PP_SEQ_FOR_EACH(UPDATE_TEMPLATE, ~, SYNCED_COMPONENTS_SEQ);
}
