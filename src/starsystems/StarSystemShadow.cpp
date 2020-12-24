/*
 * StarSystemShadow.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include "starsystems/components/Components.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/StarSystemShadow.hpp"

void ShadowStarSystem::update() {
	// use vector capacity not size for added/changed
}

EntityReference ShadowStarSystem::getEntityReference(entt::entity entity) {
	return {starSystem, entity, registry.get<UUIDComponent>(entity).uuid};
}
