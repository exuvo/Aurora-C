/*
 * ShipHull.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include <fmt/core.h>
#include <numbers>

#include "ShipHull.hpp"
#include "MunitionHull.hpp"

#define lengthToDiameterRatio 2

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
	
	crewRequirement = 0;
	emptyMass = 0;
	maxFuelMass = 0;
	maxCargoVolume = 0;
	maxMunitionVolume = 0;
	maxSuppliesMass = 0;
	volume = 0;
	maxPartHP = 0;
	maxShieldHP = 0;
	
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
		
		maxPartHP += part->health;
		emptyMass += part->mass;
		volume += part->volume;
		crewRequirement += part->crewRequirement;
		
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
		
		Shield* shield = dynamic_cast<Shield*>(part);
		if (shield != nullptr) {
			shields.push_back(PartIndex<>(i));
			maxShieldHP += shield->capacity;
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
		
		if (part->is(PartType::Container)) {
			ContainerPart* container = static_cast<ContainerPart*>(part);
			
			if (container->cargoType == &CargoTypes::FUEL) {
				maxFuelMass += container->capacity / Resources::ROCKET_FUEL.specificVolume;
				
			} else if (container->cargoType == &CargoTypes::NUCLEAR) {
				maxFuelMass += container->capacity / Resources::NUCLEAR_FISSION.specificVolume;
				
			} else if (container->cargoType == &CargoTypes::LIFE_SUPPORT) {
				maxSuppliesMass += container->capacity / Resources::LIFE_SUPPORT.specificVolume;
				
			} else if (container->cargoType == &CargoTypes::AMMUNITION) {
				maxMunitionVolume += container->capacity;
				
			} else if (container->cargoType == &CargoTypes::NORMAL) {
				maxCargoVolume += container->capacity / Resources::LIFE_SUPPORT.specificVolume;
			}
		}
	}
	
	preferredMunitionMass = 0;
	for(auto const& [muntionHull, amount] : preferredMunitions) {
		preferredMunitionMass += muntionHull->loadedMass * amount;
	}
	
	preferredCargoMass = 0;
	for(auto const& [resourcePnt, amount] : preferredCargo) {
		preferredMunitionMass += amount;
	}
	
	// Surface Area cm² V = πr^2h, http://mathhelpforum.com/geometry/170076-how-find-cylinder-dimensions-volume-aspect-ratio.html
	double length = std::pow(std::pow(2.0, 2 * lengthToDiameterRatio) * volume / std::numbers::pi, 1.0 / 3);
	double radius = std::sqrt(volume / std::numbers::pi / length);

	surfaceArea = 2 * std::numbers::pi * radius * length + 2 * std::numbers::pi * radius * radius;
	
	//	armorWidth = 10;
	armorWidth = std::max(1UL, surfaceArea / 1000000);
	
	maxArmorHP = 0;
	for (uint8_t i=0; i < armorLayers.size(); i++) {
		maxArmorHP += armorLayers[i]->blockHP * armorWidth;
	}
	
//	surfaceArea += armor;
//	volume += armor;
//	emptyMass += armor;
	
	loadedMass = emptyMass + preferredCargoMass + preferredMunitionMass + maxFuelMass + maxSuppliesMass;
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
