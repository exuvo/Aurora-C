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
#include <array>

#include "galaxy/Resources.hpp"
#include "utils/enum.h"
#include "utils/SmallList.hpp"

BETTER_ENUM(MiningLayer, uint8_t,
	Surface, Crust, Mantle, MoltenCore
)

// Does not decrease when mined. Use minableResources for current amounts.
struct OreDeposit {
	Resource* type = nullptr;
	uint64_t amount = 0;
	bool discovered = false;
};

struct PlanetComponent {
	uint64_t freshWater = 0;
	uint64_t seaWater = 0;
	uint64_t pollutedWater = 0;
	uint64_t usableLandArea = 0; // km²
	uint64_t arableLandArea = 0; // km² (subtracts from usable when used)
	uint64_t blockedLandArea = 0;
	uint8_t gravity = 100; // percentage of earth
	uint16_t atmosphericDensity = 1225; // g/m³ at 1013.25 hPa (abs) and 15°C
	uint8_t atmospheBreathability = 100; // percentage
	uint16_t temperature = 20; // celcius
	SmallList<OreDeposit> oreDeposits[MiningLayer::_size_constant];
	// Sum of discovered deposits. [Layer, Ore, Amount]
	SmallList<std::array<uint64_t, Resources::size_ores>, 32> minableResources[MiningLayer::_size_constant];
	
	uint64_t cleanWater() {
		return freshWater + seaWater;
	}
};

struct Shipyard;
struct ShipHull;

struct ShipyardSlipway {
	ShipHull* hull;
	uint64_t hullCost[Resources::size_construction];
	uint64_t usedResources[Resources::size_construction];
	
	uint64_t totalUsedResources() {
		return std::accumulate(usedResources, usedResources + Resources::size_construction, 0l);
	}
	
	uint64_t totalCost() {
		return std::accumulate(hullCost, hullCost + Resources::size_construction, 0l);
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
	ShipyardLocation* location = nullptr;
	ShipyardType* type = nullptr;
	uint64_t capacity = 1000; // In cm³
	float fuelCostPerMass = 0.0; //kg fuel per kg of hull to launch into space
	uint64_t buildRate = 0; // kg per hour
	ShipHull* tooledHull = nullptr;
	std::vector<ShipyardSlipway> slipways;
	
	ShipyardModification* modificationActivity = nullptr;
	uint32_t modificationRate = 1000; // kg per hour
	uint64_t modificationProgress = 0;
	
	Shipyard() {};
	
	Shipyard(ShipyardLocation* location, ShipyardType* type): location(location), type(type) {
		buildRate = location->baseBuildrate;
		
		if (location == &ShipyardLocations::TERRESTIAL) {
			fuelCostPerMass = 1.0;
		}
	};
};

struct Building {
	std::string name;
	uint64_t cost[Resources::size_construction];
	
	Building(const std::string name): name(name) {};
};

struct TerrestrialBuilding: public Building {
	TerrestrialBuilding(const std::string name): Building(name) {};
};
struct OrbitalBuilding: public Building {
	OrbitalBuilding(const std::string name): Building(name) {};
};

struct BuildingState {
	uint32_t requestedPower = 0;
	uint32_t givenPower = 0;
	uint32_t upkeep[Resources::size];
	uint32_t givenResources[Resources::size];
};

BETTER_ENUM(DistrictType, uint8_t,
	Housing, Farming, Industry, Power, Mining
)

struct District {
	std::string name;
	DistrictType type;
	
	District(const std::string name, DistrictType type): name(name), type(type) {};
};

struct Districts {
	static inline District HousingLowDensity { "Low density housing", DistrictType::Housing };
	static inline District HousingHighDensity { "High density housing", DistrictType::Housing };
	
	static inline District Farm { "Farm", DistrictType::Farming };
	
	static inline District GeneralIndustry { "General industry", DistrictType::Industry };
	static inline District RefineryBlastFurnace { "Blast furnace", DistrictType::Industry };
	static inline District RefineryArcFurnace { "Arc furnace", DistrictType::Industry };
	static inline District RefinerySmeltery { "Glass smeltery", DistrictType::Industry };
	static inline District RefinerySemiconductorFab { "Semiconductor fabrication plant", DistrictType::Industry };
	static inline District RefineryEnricher { "Uranium enricher", DistrictType::Industry };
	static inline District RefineryChemicalPlant { "Chemical plant", DistrictType::Industry };
	static inline District RefineryFuelRefinery { "Fuel refinery", DistrictType::Industry };
	static inline District RefineryLithium { "Lithium refinery", DistrictType::Industry };
	
	static inline District PowerSolar { "Solar power plant", DistrictType::Power };
	static inline District PowerCoal { "Coal power plant", DistrictType::Power };
	static inline District PowerFission { "Nuclear fission power plant", DistrictType::Power };
	static inline District PowerFusion { "Fusion power plant", DistrictType::Power };
	
	static inline District MineSurface { "Surface mine", DistrictType::Mining };
	static inline District MineCrust { "Mine", DistrictType::Mining };
	static inline District MineMantle { "Mantle extractor", DistrictType::Mining };
	static inline District MineMoltenCore { "Molten core pump", DistrictType::Mining };
	
	static inline constexpr const District* ALL[] { &HousingLowDensity, &HousingHighDensity, &Farm,
	                                                &GeneralIndustry, &RefineryBlastFurnace, &RefineryArcFurnace,
	                                                &RefinerySmeltery, &RefinerySemiconductorFab, &RefineryEnricher,
	                                                &RefineryChemicalPlant, &RefineryFuelRefinery, &RefineryLithium,
	                                                &PowerSolar, &PowerCoal, &PowerFission, &PowerFusion,
	                                                &MineSurface, &MineCrust, &MineMantle, &MineMoltenCore };
	
	static inline constexpr const size_t size = ARRAY_LENGTH(ALL);
};

struct ColonyComponent {
	uint64_t population = 0;
	uint64_t housingLandArea = 0;
	uint64_t farmingLandArea = 0;
	uint64_t industrialLandArea = 0; // pollutes water
	uint64_t miningLandArea = 0; // pollutes water
	uint16_t districtAmounts[Districts::size];
	SmallList<TerrestrialBuilding*, 32> buildings;
	SmallList<OrbitalBuilding*, 32> orbitalBuildings;
	SmallList<BuildingState*, 32> buildingStates;
	SmallList<BuildingState*, 32> orbitalBuildingStates;
	SmallList<Shipyard, 8> shipyards;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_COLONYCOMPONENTS_HPP_ */
