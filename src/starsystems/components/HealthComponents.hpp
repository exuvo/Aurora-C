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
	
	ShieldComponent(ShipHull& hull, PartStatesComponent& partStates) {
		assert(hull.shields.size() > 0);
		
		for (const PartIndex<Shield>& partIdx : hull.shields) {
			shieldHP += partStates.charged[hull.getPartStateIndex<ChargedPartState>(partIdx)].charged;
		}
	};
};

struct ArmorComponent {
	SmallList<SmallList<uint8_t, 16>, 16> armor; // [layer][armor column] = hp
	
	ArmorComponent(ShipHull& hull) {
//		armor = Array<UByteArray>(hull.armorLayers, { layer -> UByteArray(hull.getArmorWidth(), { hull.armorBlockHP[layer] }) }) // 1 armor block per m2
		
		armor.reserve(hull.armorLayers.size());
		
		for (int i=0; i < hull.armorLayers.size(); i++) {
			armor[i] = std::move(SmallList<uint8_t, 16>());
			armor[i].reserve(hull.armorWidth);
			
			uint8_t hp = hull.armorLayers[i]->blockHP;
			
			for (int j=0; j < hull.armorWidth; j++) {
				armor[i][j] = hp;
			}
		}
	}
	
	ArmorComponent(AdvancedMunitionHull& hull) {
		
	}
	
	uint16_t getTotalHP();
};

struct PartsHPComponent {
	uint16_t totalPartHP = 0;
	SmallList<uint8_t, 64> partHP;
	std::multimap<uint64_t, PartIndex<Part>> damageableParts;
	uint64_t damageablePartsMaxVolume = 0;
	
	PartsHPComponent(ShipHull& hull): damageableParts(std::multimap<uint64_t, PartIndex<Part>>()) {
		
	}
	
	uint8_t getPartHP(PartIndex<Part> part);
	void setPartHP(PartIndex<Part> part, uint8_t hp);
};

struct HPComponent {
	uint16_t hp = 0;
	
	HPComponent(uint16_t hp): hp(hp) {};
	HPComponent(AdvancedMunitionHull& hull) {
		for(const Part* part : hull.parts) {
			hp += part->health;
		}
	};
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_HEALTHCOMPONENTS_HPP_ */
