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
