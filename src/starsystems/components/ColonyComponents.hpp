/*
 * ColonyComponent.hpp
 *
 *  Created on: Jan 2, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_COLONYCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_COLONYCOMPONENTS_HPP_

#include <stdint.h>
#include <string>
#include <numeric>

#include "galaxy/Resources.hpp"

struct Shipyard;
struct ShipHull;

struct Building {
	std::string name;
	uint64_t cost[Resources::size];
};

struct ShipyardSlipway {
	ShipHull* hull;
	uint64_t hullCost[Resources::size];
	uint64_t usedResources[Resources::size];
	
	uint64_t totalUsedResources() {
		return std::accumulate(usedResources, usedResources + Resources::size, 0l);
	}
	
	uint64_t totalCost() {
		return std::accumulate(hullCost, hullCost + Resources::size, 0l);
	}
	
	uint32_t progress() {
		uint64_t usedResources = totalUsedResources();
		
		if (usedResources == 0L) {
			return 0;
		}
		
		return (100L * usedResources) / totalCost();
	}
};

struct ShipyardLocation {
	const char abbreviation[4];
	const uint16_t baseBuildrate;
	const uint16_t modificationMultiplier;
	
	bool operator==(const ShipyardLocation& o) const {
		return this == &o;
	}
};

struct ShipyardLocations {
	static inline constexpr ShipyardLocation ORBITAL { "ORB", 100, 120 };
	static inline constexpr ShipyardLocation TERRESTIAL { "GND", 150, 100 };
};

struct ShipyardType {
	const char abbreviation[4];
	const uint16_t modificationMultiplier;
	
	bool operator==(const ShipyardType& o) const {
		return this == &o;
	}
};

struct ShipyardTypes {
	static inline constexpr ShipyardType CIVILIAN { "CIV", 100 };
	static inline constexpr ShipyardType MILITARY { "MIL", 150 };
};

struct ShipyardModificationEnum {
	const char name[16];
	
	bool operator==(const ShipyardModificationEnum& o) const {
		return this == &o;
	}
};

struct ShipyardModifications {
	static inline constexpr ShipyardModificationEnum RETOOL { "Retool" };
	static inline constexpr ShipyardModificationEnum EXPAND_CAPACITY { "Expand capacity" };
	static inline constexpr ShipyardModificationEnum ADD_SLIPWAY { "Add slipway" };
};

struct ShipyardModification {
	virtual ~ShipyardModification() = default;
	
	virtual uint64_t getCost(Shipyard& shipyard) = 0;
	virtual void complete(Shipyard& shipyard) = 0;
	virtual std::string getDescription() = 0;
};

class ShipyardModificationExpandCapacity: public ShipyardModification {
	uint64_t addedCapacity;
	
	ShipyardModificationExpandCapacity(uint64_t addedCapacity): addedCapacity(addedCapacity) {};
	
	virtual uint64_t getCost(Shipyard& shipyard);
	virtual void complete(Shipyard& shipyard);
	virtual std::string getDescription();
};

class ShipyardModificationRetool: public ShipyardModification {
	ShipHull& assignedHull; 
	
	ShipyardModificationRetool(ShipHull& assignedHull): assignedHull(assignedHull) {};
	
	virtual uint64_t getCost(Shipyard& shipyard);
	virtual void complete(Shipyard& shipyard);
	virtual std::string getDescription();
};

class ShipyardModificationAddSlipway: public ShipyardModification {
	virtual uint64_t getCost(Shipyard& shipyard);
	virtual void complete(Shipyard& shipyard);
	virtual std::string getDescription();
};

struct Shipyard {
	ShipyardLocation* location;
	ShipyardType* type;
	uint64_t capacity = 1000; // In cm³
	float fuelCostPerMass = 0.0; //kg fuel per kg of hull to launch into space
	uint64_t buildRate; // kg per hour
	ShipHull* tooledHull = nullptr;
	std::vector<ShipyardSlipway> slipways;
	
	ShipyardModification* modificationActivity = nullptr;
	uint32_t modificationRate = 1000; // kg per hour
	uint64_t modificationProgress = 0;
	
	Shipyard(ShipyardLocation* location, ShipyardType* type): location(location), type(type) {
		buildRate = location->baseBuildrate;
		
		if (location == &ShipyardLocations::TERRESTIAL) {
			fuelCostPerMass = 1.0;
		}
	};
};

struct PlanetComponent {
	uint64_t cleanWater = 0;
	uint64_t pollutedWater = 0;
	uint64_t usableLandArea = 0; // km²
	uint64_t arableLandArea = 0; // km² (subtracts from usable when used)
	uint64_t blockedLandArea = 0;
	uint8_t gravity = 100; // percentage of earth
	uint16_t atmosphericDensity = 1225; // g/m³ at 1013.25 hPa (abs) and 15°C
	uint8_t atmospheBreathability = 100; // percentage
	uint16_t temperature = 20; // celcius
	uint64_t minableResources[Resources::size];
	uint32_t resourceAccessibility[Resources::size];
};

struct ColonyComponent {
	uint64_t population = 0;
	uint64_t housingLandArea = 0;
	uint64_t farmingLandArea = 0;
	uint64_t industrialLandArea = 0; // pollutes water
	uint64_t miningLandArea = 0; // pollutes water
	std::vector<Building> buildings;
	std::vector<Shipyard> shipyards;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_COLONYCOMPONENTS_HPP_ */
