/*
 * ShipHull.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include <fmt/core.h>

#include "ShipHull.hpp"

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<FueledPartState>() {
	return partStateIndexes[0];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<PoweringPartState>() {
	return partStateIndexes[1];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<PoweredPartState>() {
	return partStateIndexes[2];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<PassiveSensorState>() {
	return partStateIndexes[3];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<ChargedPartState>() {
	return partStateIndexes[4];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<AmmunitionPartState>() {
	return partStateIndexes[5];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<WeaponPartState>() {
	return partStateIndexes[6];
}

template<>
std::vector<PartStateIndex>& ShipHull::getPartStateIndex<TargetingComputerState>() {
	return partStateIndexes[7];
}

void ShipHull::calculateCachedValues() {
	shields.clear();
	thrusters.clear();
	targetingComputers.clear();
	
	for (size_t i=0; i < ARRAY_LEN(partStateIndexes); i++) {
		partStateIndexes[i].clear();
	}
	
	uint8_t nextFueledIdx = 0;
	uint8_t nextPoweringIdx = 0;
	uint8_t nextPoweredIdx = 0;
	uint8_t nextChargedIdx = 0;
	uint8_t nextAmmunitionIdx = 0;
	uint8_t nextWeaponIdx = 0;
	uint8_t nextTargetingComputerIdx = 0;
	uint8_t nextPassiveSensordIdx = 0;
	
	for (size_t i=0; i < parts.size(); i++) {
		Part* part = parts[i];
		
		if (part->is(PartType::Thrusting)) {
			thrusters.push_back(PartIndex<>(i));
		}
		
		if (part->is(PartType::Fueled)) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<FueledPartState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextFueledIdx++;
		}
		
		if (part->is(PartType::Powering)) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<PoweringPartState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextPoweringIdx++;
		}
		
		if (part->is(PartType::Powered)) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<PoweredPartState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextPoweredIdx++;
		}
		
		if (part->is(PartType::Charged)) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<ChargedPartState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextChargedIdx++;
		}
		
		if (part->is(PartType::Ammunition)) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<AmmunitionPartState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextAmmunitionIdx++;
		}
		
		if (part->is(PartType::Weapon)) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<WeaponPartState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextWeaponIdx++;
		}
		
		if (dynamic_cast<Shield*>(part) != nullptr) {
			shields.push_back(PartIndex<>(i));
		}
		
		if (dynamic_cast<TargetingComputer*>(part) != nullptr) {
			targetingComputers.push_back(PartIndex<>(i));
			
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<TargetingComputerState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextTargetingComputerIdx++;
		}
		
		if (dynamic_cast<PassiveSensor*>(part) != nullptr) {
			std::vector<PartStateIndex>& partStateIdxs = getPartStateIndex<PassiveSensorState>();
			partStateIdxs.reserve(1 + i);
			partStateIdxs[i] = nextPassiveSensordIdx++;
		}
	}
}

std::string ShipHull::toString() const {
	if (parentHull == nullptr) {
		return fmt::format("{} {}", name, daysToYear(designDay));
	}
	
	return fmt::format("{} {}-{}", name, daysToYear(parentHull->designDay), daysToSubYear(designDay));
}

std::ostream& operator<< (std::ostream& out, const ShipHull& hull) {
	return out << hull.toString();
}
