/*
 * ColonyComponents.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include <fmt/core.h>

#include "ColonyComponents.hpp"
#include "utils/Utils.hpp"

uint64_t ShipyardModificationExpandCapacity::getCost(Shipyard& shipyard) {
	return (addedCapacity * shipyard.slipways.size() * shipyard.type->modificationMultiplier * shipyard.location->modificationMultiplier) / 10;
}

void ShipyardModificationExpandCapacity::complete(Shipyard& shipyard) {
	shipyard.capacity += addedCapacity;
}

std::string ShipyardModificationExpandCapacity::getDescription() {
	return fmt::format("Expanding capacity by {}", volumeToString(addedCapacity));
}

uint64_t ShipyardModificationRetool::getCost(Shipyard& shipyard) {
	return (shipyard.capacity * shipyard.slipways.size() * shipyard.type->modificationMultiplier * shipyard.location->modificationMultiplier) / 100;
}

void ShipyardModificationRetool::complete(Shipyard& shipyard) {
	shipyard.tooledHull = &assignedHull;
}

std::string ShipyardModificationRetool::getDescription() {
	return fmt::format("Retooling to {}", assignedHull);
}

uint64_t ShipyardModificationAddSlipway::getCost(Shipyard& shipyard) {
	return (shipyard.capacity * shipyard.type->modificationMultiplier * shipyard.location->modificationMultiplier) / 10;
}

void ShipyardModificationAddSlipway::complete(Shipyard& shipyard) {
	shipyard.slipways.push_back(ShipyardSlipway());
}

std::string ShipyardModificationAddSlipway::getDescription() {
	return "Adding slipway";
}
