/*
 * StarSystemShadow.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include "starsystems/components/Components.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/StarSystemShadow.hpp"

void operator |=(std::vector<bool> a, std::vector<bool> b) {
	size_t size = a.capacity();
	for (size_t i = 0; i < size; i++) {
		a[i] = a[i] | b[i];
	}
}

void operator &=(std::vector<bool> a, std::vector<bool> b) {
	size_t size = a.capacity();
	for (size_t i = 0; i < size; i++) {
		a[i] = a[i] & b[i];
	}
}

// And not
void operator %=(std::vector<bool> a, std::vector<bool> b) {
	size_t size = a.capacity();
	for (size_t i = 0; i < size; i++) {
		a[i] = a[i] & ~b[i];
	}
}

void ShadowStarSystem::update() {
	// use vector capacity not size for added/changed
	ShadowStarSystem* shadow = starSystem->shadow;
	tmp.reserve(std::max({added.capacity(), changed.capacity(), deleted.capacity(), shadow->added.capacity(), shadow->changed.capacity(), shadow->deleted.capacity()}));
	
	tmp = added;
	tmp |= deleted;
	added %= tmp;
	changed %= tmp;
	deleted %= tmp;
	
	
	
	
	if (quadtreeShipsChanged || starSystem->shadow->quadtreeShipsChanged) {
		profilerEvents.start("copy quadtree ships");
		quadtreeShips = starSystem->systems->spatialPartitioningSystem->tree;
//		memcpy(&quadtreeShips, &starSystem->systems->spatialPartitioningSystem->tree, sizeof(quadtreeShips));
		profilerEvents.end();
	}
	
	if (quadtreePlanetoidsChanged || starSystem->shadow->quadtreePlanetoidsChanged) {
		profilerEvents.start("copy quadtree planetoids");
		quadtreePlanetoids = starSystem->systems->spatialPartitioningPlanetoidsSystem->tree;
//		memcpy(&quadtreePlanetoids, &starSystem->systems->spatialPartitioningPlanetoidsSystem->tree, sizeof(quadtreePlanetoids));
		profilerEvents.end();
	}
}

EntityReference ShadowStarSystem::getEntityReference(entt::entity entity) {
	return {starSystem, entity, registry.get<UUIDComponent>(entity).uuid};
}
