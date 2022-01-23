/*
 * ColonyComponents.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include <fmt/core.h>

#include "ColonyComponents.hpp"
#include "galaxy/ShipHull.hpp"
#include "utils/Utils.hpp"

void ShipyardSlipway::build(const ShipHull& newHull) {
	if (hull != nullptr) {
		throw std::runtime_error("Already building a ship");
	}
	
	hull = &newHull;
	
	for (size_t i = 0; i < Resources::size_construction; i++) {
		usedResources[i] = 0;
		
		auto find = newHull.cost.find(*Resources::ALL_CONSTRUCTION[i]);
		
		if (find != newHull.cost.end()) {
			hullCost[i] = find->second;
		} else {
			hullCost[i] = 0;
		}
	}
};

uint64_t ShipyardModificationExpandCapacity::getCost(Shipyard& shipyard) const {
	return (addedCapacity * shipyard.slipways.size() * shipyard.type->modificationMultiplier * shipyard.location->modificationMultiplier) / 10;
}

void ShipyardModificationExpandCapacity::complete(Shipyard& shipyard) {
	shipyard.capacity += addedCapacity;
}

std::string ShipyardModificationExpandCapacity::getDescription() const {
	return fmt::format("Expanding capacity by {}", volumeToString(addedCapacity));
}

uint64_t ShipyardModificationRetool::getCost(Shipyard& shipyard) const {
	return (shipyard.capacity * shipyard.slipways.size() * shipyard.type->modificationMultiplier * shipyard.location->modificationMultiplier) / 100;
}

void ShipyardModificationRetool::complete(Shipyard& shipyard) {
	shipyard.tooledHull = &assignedHull;
}

std::string ShipyardModificationRetool::getDescription() const {
	return fmt::format("Retooling to {}", assignedHull);
}

uint64_t ShipyardModificationAddSlipway::getCost(Shipyard& shipyard) const {
	return (shipyard.capacity * shipyard.type->modificationMultiplier * shipyard.location->modificationMultiplier) / 10;
}

void ShipyardModificationAddSlipway::complete(Shipyard& shipyard) {
	shipyard.slipways.push_back(ShipyardSlipway());
}

std::string ShipyardModificationAddSlipway::getDescription() const {
	return "Adding slipway";
}
