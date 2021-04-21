/*
 * MunitionHull.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_MUNITIONHULL_HPP_
#define SRC_GALAXY_MUNITIONHULL_HPP_

#include <vector>

#include "galaxy/ShipParts.hpp"
#include "galaxy/Resources.hpp"
#include "utils/enum.h"

struct ArmorLayer;

BETTER_ENUM(DamagePattern, uint8_t,
  KINETIC, EXPLOSIVE, LASER
)

template<class partType = Part>
struct MunitionPartIndex {
	uint8_t idx;
	
	MunitionPartIndex(uint8_t idx): idx(idx) {}
	
	template<class other>
	MunitionPartIndex(MunitionPartIndex<other> p): idx(p.idx) {}
	
	inline uint8_t operator() () const {
		return idx;
	}
	
	inline operator uint8_t () const {
		return idx;
	}
};

struct MunitionHull {
	ResourcePnt storageType;
	std::string name;
	uint32_t designDay = 0;
	bool locked;
	bool obsolete;
	uint64_t damage; // Joules
	uint8_t radius; // cm
	uint16_t loadedMass; // kg
	uint32_t volume; // cm³
	
	virtual ~MunitionHull() = default;
	virtual void calculateCachedValues() = 0;
};

struct SimpleMunitionHull: MunitionHull {
	uint8_t health;
	DamagePattern damagePattern = DamagePattern::KINETIC;
	
	//TODO port
	virtual void calculateCachedValues() override;
};

struct AdvancedMunitionHull: MunitionHull {
	std::vector<Part*> parts;
	std::vector<ArmorLayer*> armorLayers; // Each layer is 1 centimetre of armor
	std::vector<MunitionPartIndex<ThrustingPart>> thrusters;
	
	//TODO port
	virtual void calculateCachedValues() override;
};

#endif /* SRC_GALAXY_MUNITIONHULL_HPP_ */
