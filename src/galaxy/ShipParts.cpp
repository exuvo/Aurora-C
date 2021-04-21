/*
 * ShipParts.cpp
 *
 *  Created on: Jan 9, 2021
 *      Author: exuvo
 */

#include <algorithm>
#include <numbers>
#include <fmt/core.h>

#include "ShipParts.hpp"
#include "utils/Utils.hpp"

bool Part::is(PartType type) const {
	return types & (1 << type);
}

bool operator &(const Part& part, PartType type) {
	return part.is(type);
}

void Part::calculateCachedValues() {
	mass = std::accumulate(cost.begin(), cost.end(), 0l, [](uint64_t sum, const std::pair<ResourcePnt, uint64_t>& pair) {
		return sum + pair.second;
	});
	
	if (mass == 0) {
		mass = 1;
	}
	
	volume = std::accumulate(cost.begin(), cost.end(), 0l, [](uint64_t sum, const std::pair<ResourcePnt, uint64_t>& pair) {
		return sum + pair.second * pair.first->specificVolume;
	});
	
	if (volume == 0) {
		volume = 1;
	}
}

std::string Part::toString() const {
	if (name.size() > 0) {
//		type_name<Component>();
			return fmt::format("{} {}", type_name(this), name);
		}
		
		return fmt::format("{}", type_name(this));
}

// 1 gram of fissile material yields about 1 megawatt-day (MWd) of heat energy.  https://www.nuclear-power.net/nuclear-power-plant/nuclear-fuel/fuel-consumption-of-conventional-reactor/
// 1 MWd = 1 second of 86,400,000,000W
uint32_t FissionReactor::calculateFuelTime(uint32_t power, uint8_t efficiency) {
	return (86400000000000L / power) * (efficiency / 100.0);
}

std::ostream& operator<< (std::ostream& out, const Part& part) {
	return out << part.toString();
}

// Diffraction limited radial beam divergence in radians
	// https://www.quora.com/Is-the-light-from-lasers-reduced-by-the-inverse-square-law-as-distance-grows-similar-to-other-light-sources
	// https://en.wikipedia.org/wiki/Beam_divergence
double BeamWeapon::getRadialDivergence() {
	return (waveLength->length / 1000000000.0) / (std::numbers::pi* 1000 * aperature / 2);
}

// We are always outside rayleight range so beam width is linear to distance
	// in m of beam radius
double BeamWeapon::getBeamRadiusAtDistance(uint64_t distance) {
	return distance * std::tan(getRadialDivergence());
}

// in m²
double BeamWeapon::getBeamArea(uint64_t distance) {
	return std::numbers::pi * std::pow(getBeamRadiusAtDistance(distance), 2.0);
}

// in watts of delivered energy
uint64_t BeamWeapon::getDeliveredEnergyTo1MSquareAtDistance(uint64_t distance) {
	return (efficiency * capacity) / (100 * std::max(1ul, (uint64_t) getBeamArea(distance)));
}

