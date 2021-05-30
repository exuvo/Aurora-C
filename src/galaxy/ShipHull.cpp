/*
 * ShipHull.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include <fmt/core.h>

#include "ShipHull.hpp"

void ShipHull::calculateCachedValues() {
	shields.clear();
	thrusters.clear();
	targetingComputers.clear();
	
	for (size_t i=0; i < parts.size(); i++) {
		Part* part = parts[i];
		
		if (typeid(part) == typeid(Shield)) {
			shields.push_back(PartIndex<>(i));
			
		} else if (typeid(part) == typeid(ThrustingPart)) {
			thrusters.push_back(PartIndex<>(i));
			
		} else if (typeid(part) == typeid(TargetingComputer)) {
			targetingComputers.push_back(PartIndex<>(i));
		}
	}
	
	//TODO partStateIndexes
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
