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
#include "starsystems/components/StrategicIconComponent.hpp"
#include "starsystems/components/PowerComponent.hpp"
#include "starsystems/components/ColonyComponents.hpp"

struct MunitionHull;

struct HullClass {
	std::string name;
	std::string code;
};

struct ArmorLayer {
	std::string name;
	uint8_t blockHP;
	uint16_t energyPerDamage;
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
	std::unordered_map<Resource*, uint64_t> preferredCargo;
	std::map<MunitionHull*, uint32_t> preferredMunitions;
	std::unordered_map<PartIndex<WeaponPart>, MunitionHull*> preferredPartMunitions;
	const PowerScheme* powerScheme = &PowerSchemes::SOLAR_BATTERY_REACTOR;
	std::unordered_map<PartIndex<TargetingComputer>, std::vector<PartIndex<WeaponPart>>> defaultWeaponAssignments;
	
	std::vector<PartIndex<Shield>> shields;
	std::vector<PartIndex<ThrustingPart>> thrusters;
	std::vector<PartIndex<TargetingComputer>> targetingComputers;
	
	void calculateCachedValues();
	std::string toString() const;
	static inline constexpr double LengthToDiameterRatio = 2.0;
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
