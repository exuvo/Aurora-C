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
#include "utils/SmallList.hpp"

#include <Refureku/NativeProperties.h>
#include "refureku/HealthComponents.rfk.h"

struct ShieldComponent {
	uint64_t shieldHP = 0;
	
	ShieldComponent(ShipHull& hull, PartStatesComponent& partStates) {
		assert(hull.shields.size() > 0);
		
		for (const PartIndex<Shield>& partIdx : hull.shields) {
			shieldHP += partStates.charged[hull.getPartStateIndex<ChargedPartState>(partIdx)].charge;
		}
	};
};

struct ArmorComponent {
	SmallList<SmallList<uint8_t, 16>, 16> armor; // [layer][armor column] = hp
	
	ArmorComponent(ShipHull& hull) {
//		armor = Array<UByteArray>(hull.armorLayers, { layer -> UByteArray(hull.getArmorWidth(), { hull.armorBlockHP[layer] }) }) // 1 armor block per m2
		
		armor.reserve(hull.armorLayers.size());
		
		for (size_t i=0; i < hull.armorLayers.size(); i++) {
			armor[i] = std::move(SmallList<uint8_t, 16>());
			armor[i].reserve(hull.armorWidth);
			
			uint8_t hp = hull.armorLayers[i]->blockHP;
			
			for (size_t j=0; j < hull.armorWidth; j++) {
				armor[i][j] = hp;
			}
		}
	}
	
	ArmorComponent(AdvancedMunitionHull& hull) {
		
	}
	
	uint16_t getTotalHP();
};

struct DamagablePart {
	uint64_t volumeSum = 0;
	PartIndex<Part> partIdx = 0;
	
	bool operator< (uint64_t randomVolume) const {
		return volumeSum < randomVolume;
	}
};

struct DamagablePartWithIndex {
	PartIndex<Part> partIdx;
	uint8_t idx;
};

struct PartsHPComponent {
	SmallList<uint8_t, 64> partHP;
	uint16_t totalPartHP = 0;
	/* Cache of damagable parts weighted by part volume.
	 * Tree of volume sum so far, [0] = volume of part 0, [1] = [0] + volume of part 1 ... Ordered by large parts last.
	 * Key values always strictly ordered, can binary search.
	 */
	SmallList<DamagablePart, 64> damageableParts;
	uint64_t damageablePartsMaxVolumeSum;
	
	PartsHPComponent(const ShipHull& hull) {
		partHP.reserve(hull.parts.size());
		for (size_t i = 0; i < hull.parts.size(); i++) {
			Part* part = hull.parts[i];
			partHP[i] = part->health;
			totalPartHP += part->health;
		}
		
		damageableParts.reserve(hull.parts.size());
		rebuildDamagableParts(hull);
	}
	
	void rebuildDamagableParts(const ShipHull& hull) {
		damageableParts.clear();
		damageablePartsMaxVolumeSum = 0;
		
		for (size_t i = 0; i < hull.parts.size(); i++) {
			Part* part = hull.parts[i];
			damageableParts.push_back(damageablePartsMaxVolumeSum += part->volume, i);
		}
	}
	
	uint8_t getPartHP(PartIndex<Part> partIdx) const {
		return partHP[partIdx];
	}
	
	DamagablePartWithIndex getDamagablePart(uint64_t randomVolume) const {
		decltype(damageableParts)::const_iterator found = std::lower_bound(damageableParts.begin(), damageableParts.end(), randomVolume);
		
		if (found != damageableParts.end()) {
			return { found->partIdx, found - damageableParts.begin() };
		}
		
		throw std::range_error("No part found");
	}
	
	void setPartHP(DamagablePartWithIndex damagablePartWithIndex, uint8_t hp, const ShipHull& hull) {
		PartIndex<Part> partIdx = damagablePartWithIndex.partIdx;
		uint8_t oldHP = partHP[partIdx];
		partHP[partIdx] = hp;
		
		if (hp == 0) { // Destroyed, lower volume sum of parts above dead part (assumed fewer than other direction)
			uint32_t volume = hull.parts[partIdx]->volume;
			damageablePartsMaxVolumeSum -= volume;
			
			size_t i = damagablePartWithIndex.idx;
			
			while (i < damageableParts.size()) {
				damageableParts[i++].volumeSum -= volume;
			}
			
		} else if (oldHP == 0) { // Repaired
			rebuildDamagableParts(hull);
		}
	}
	
	void setPartHP(PartIndex<Part> partIdx, uint8_t hp, const ShipHull& hull) {
		uint8_t oldHP = partHP[partIdx];
		partHP[partIdx] = hp;
		
		if (hp == 0) { // Destroyed, lower volume sum of parts above dead part (assumed fewer than other direction)
			uint32_t volume = hull.parts[partIdx]->volume;
			damageablePartsMaxVolumeSum -= volume;
			
			size_t i = 0;
			while (damageableParts[i].partIdx != partIdx) {
				i++;
				assert(i < damageableParts.size());
			}
			
			while (i < damageableParts.size()) {
				damageableParts[i++].volumeSum -= volume;
			}
			
		} else if (oldHP == 0) { // Repaired
			rebuildDamagableParts(hull);
		}
	}
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
