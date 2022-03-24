/*
 * ShipHull.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_SHIPHULL_HPP_
#define SRC_GALAXY_SHIPHULL_HPP_

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <fmt/format.h>

#include "galaxy/ShipParts.hpp"
#include "galaxy/ShipHullClass.hpp"
#include "galaxy/Resources.hpp"
#include "starsystems/components/StrategicIconComponent.hpp"
#include "starsystems/components/PowerComponent.hpp"
#include "starsystems/components/ColonyComponents.hpp"
#include "starsystems/components/PartStatesComponent.hpp"

struct MunitionHull;

// self-regenerating organic armor, slowly drains ship life-support
struct ArmorLayer {
	std::string name;
	uint16_t energyPerDamage;
	uint8_t blockHP;
	uint8_t kineticResistance = 100; // 0-255%
	uint8_t thermalResistance = 100;
	uint8_t explosiveResistance = 100;
	uint8_t absorption = 0; // % of thermal damage converted to power
};

struct ShipHull {
	std::string name = "";
	HullClass* hullClass = nullptr;
	ShipHull* parentHull = nullptr;
	std::vector<ShipHull*> derivatives;
	StrategicIcon icon;
	uint32_t designDay = 0;
	bool locked = false;
	bool obsolete = false;
	const ShipyardType* requiredShipYardType = &ShipyardTypes::CIVILIAN;
	std::string comment = "";
	std::vector<Part*> parts;
	std::vector<ArmorLayer*> armorLayers; // Each layer is 1 centimetre of armor
	std::unordered_map<ResourcePnt, uint64_t> preferredCargo;
	std::map<MunitionHull*, uint32_t> preferredMunitions;
	std::unordered_map<PartIndex<WeaponPart>, MunitionHull*> preferredPartMunitions;
	const PowerScheme* powerScheme = &PowerSchemes::SOLAR_BATTERY_REACTOR;
	std::unordered_map<PartIndex<TargetingComputer>, std::vector<PartIndex<WeaponPart>>> defaultWeaponAssignments;
	
	std::vector<PartIndex<Shield>> shields;
	std::vector<PartIndex<ThrustingPart>> thrusters;
	std::vector<PartIndex<TargetingComputer>> targetingComputers;
	
	uint16_t crewRequirement;
	uint32_t emptyMass; // kg
	uint32_t loadedMass;
	uint32_t preferredCargoMass;
	uint32_t preferredMunitionMass;
	uint32_t maxFuelMass;
	uint32_t maxLifeSupportMass;
	uint32_t maxCargoVolume;
	uint32_t maxMunitionVolume;
	uint64_t volume; // cm³
	uint64_t surfaceArea; // cm²
	uint8_t armorWidth;
	uint16_t maxPartHP;
	uint32_t maxArmorHP;
	uint32_t maxShieldHP;
	std::map<Resource, uint32_t> cost;
	
	void calculateCachedValues();
	std::string toString() const;
	static inline constexpr double LengthToDiameterRatio = 2.0;
	
	template<typename T>
	std::vector<PartStateIndex>& getPartStateIndex();
	
	template<typename T>
	PartStateIndex getPartStateIndex(PartIndex<Part> partIndex) {
		return getPartStateIndex<T>()[partIndex];
	}

private:
	std::vector<PartStateIndex> partStateIndexes[PART_STATES_COUNT];
};

std::ostream& operator<< (std::ostream& out, const ShipHull& hull);

template <>
struct fmt::formatter<ShipHull> {
	// Parses format specifications
	constexpr auto parse(format_parse_context& ctx) {
		auto it = ctx.begin(), end = ctx.end();
		
		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");
		
		// Return an iterator past the end of the parsed range:
		return it;
	}
	
	template<typename FormatContext>
	auto format(const ShipHull& hull, FormatContext& ctx) {
		return format_to(ctx.out(), "{}", hull.toString());
	}
};

#endif /* SRC_GALAXY_SHIPHULL_HPP_ */
