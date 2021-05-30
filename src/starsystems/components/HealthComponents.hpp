/*
 * HealthComponents.hpp
 *
 *  Created on: May 2, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_HEALTHCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_HEALTHCOMPONENTS_HPP_

#include <map>
#include <entt/entt.hpp>

#include "galaxy/ShipHull.hpp"
#include "galaxy/MunitionHull.hpp"
#include "PartStatesComponent.hpp"
#include "utils/quadtree/SmallList.hpp"

#include <Refureku/NativeProperties.h>
#include "refureku/HealthComponents.rfk.h"

struct ShieldComponent {
	uint64_t shieldHP = 0;
	
	ShieldComponent() {};
	ShieldComponent(ShipHull& hull, PartStatesComponent& states) {
		assert(hull.shields.size() > 0);
		
		for (const PartIndex<Shield>& partIdx : hull.shields) {
//			shieldHP += partStates[partIdx][ChargedPartState].charge;
		}
	};
};

struct ArmorComponent {
	SmallList<SmallList<uint8_t, 16>, 16> armor; // [layer][armor column] = hp
	
};

struct PartsHPComponent {
	uint16_t totalPartHP = 0;
	SmallList<uint8_t, 64> partHP;
	uint64_t damageablePartsMaxVolume;
	std::multimap<uint64_t, PartIndex<Part>> damageableParts;
	
};

struct HPComponent {
	uint16_t hp = 0;
	
	HPComponent() {};
	HPComponent(uint16_t hp): hp(hp) {};
	HPComponent(AdvancedMunitionHull& hull) {
		for(const Part* part : hull.parts) {
			hp += part->health;
		}
	};
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_HEALTHCOMPONENTS_HPP_ */
