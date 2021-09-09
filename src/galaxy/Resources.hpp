/*
 * Resources.hpp
 *
 *  Created on: Jan 2, 2021
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_RESOURCES_HPP_
#define SRC_GALAXY_RESOURCES_HPP_

#include <stdint.h>
#include <vector>
#include <array>
#include <algorithm>

#include "utils/Utils.hpp"

// Density in g/cm³.  1 g/cm³ = 1000 kg/m³
constexpr uint32_t densityToVolume(float density) {
	return 1000 / density;
}

// Weight should be stored in kg, Volume in cm³
// specificVolume in cm³/kg
struct Resource {
	const uint32_t specificVolume;
	
	constexpr Resource(int specificVolume) : specificVolume(specificVolume) {};
	constexpr Resource(double specificVolume) : specificVolume(densityToVolume(specificVolume)) {};
	
	bool operator==(const Resource& o) const {
		return this == &o;
	}
};

// Always store these in pointers, otherwise comparisons will fail. TODO private constructor?
struct Resources {
	// No storage requirements
	//TODO types for buildings, stations, spaceship hull, laser, railguns, missile explosives, armor
	static inline constexpr Resource GENERIC { 5.0 }; // Steel(11.7 g/cm³), Concrete(2.4 g/cm³), Carbonfiber, Glass(2.5 g/cm³), Ceramics(4 g/cm³)
	static inline constexpr Resource METAL_LIGHT { 3.6 }; // Aluminium(2.7 g/cm³), Titanium(4.5 g/cm³)
	static inline constexpr Resource METAL_CONDUCTIVE { 12.0 }; // Copper(9 g/cm³), Gold(19 g/cm³)
	static inline constexpr Resource SEMICONDUCTORS { 2.3 }; // Silicon(2.3 g/cm³), germanium
	static inline constexpr Resource RARE_EARTH { 7.0 }; // Neodymium(7.0 g/cm³). https://en.wikipedia.org/wiki/Rare-earth_element
	static inline constexpr Resource MAINTENANCE_SUPPLIES { 1.0 };
	static inline constexpr Resource MISSILES { 0 };
	static inline constexpr Resource SABOTS { 0 };
	// Requires radiation shielding
	static inline constexpr Resource NUCLEAR_FISSION { 15.0 }; // Uranium(19.1 g/cm³), Thorium(11.7 g/cm³)
	static inline constexpr Resource NUCLEAR_WASTE { 10.0 };
	// Requires temperature control
	// Liquid deuterium (162 kg/m³) https://en.wikipedia.org/wiki/Deuterium#Data_for_elemental_deuterium
	static inline constexpr Resource NUCLEAR_FUSION { 0.1624 }; // Deuterium-Tritium, Helium3. See 'Fusion Reactor Fuel Modes' at http://forum.kerbalspaceprogram.com/index.php?/topic/155255-12213-kspi-extended-11414-05-7-2017-support-release-thread/
	static inline constexpr Resource ROCKET_FUEL { 1.5 }; // ca 1.5 g/cm³, LOX + kerosene, LOX + H, nitrogen tetroxide + hydrazine. https://en.wikipedia.org/wiki/Rocket_propellant#Liquid_propellants
	// Requires temperature control and atmosphere
	static inline constexpr Resource LIFE_SUPPORT { 0.8 }; // Food, Water, Air
	
	static inline constexpr const Resource* ALL[] { &GENERIC, &METAL_LIGHT, &METAL_CONDUCTIVE, &SEMICONDUCTORS, &RARE_EARTH,
	                                         &MAINTENANCE_SUPPLIES, &MISSILES, &SABOTS, &NUCLEAR_FISSION, &NUCLEAR_WASTE,
	                                         &NUCLEAR_FUSION, &ROCKET_FUEL, &LIFE_SUPPORT };
	
	static inline constexpr const size_t size = ARRAY_LENGTH(ALL);
};

struct ResourcePnt {
	uint8_t idx = 0;
	
	constexpr ResourcePnt(uint8_t idx): idx(idx) {};
	constexpr ResourcePnt(const Resource* resource) {
		auto itr = std::find(Resources::ALL, Resources::ALL + ARRAY_LENGTH(Resources::ALL), resource);
		
		if (itr != std::end(Resources::ALL)) {
			idx = itr - Resources::ALL;
		} else {
			throw std::invalid_argument("Invalid resource pointer");
		}
	}
	
	const Resource* operator -> () const {
		return Resources::ALL[idx];
	}
	
	inline operator uint8_t () const {
		return idx;
	}
};

struct CargoType {
	const std::vector<Resource> resources;
	
	CargoType(const std::vector<Resource> resources) : resources(resources) {};
	
	bool operator==(const CargoType& o) const {
		return this == &o;
	}
};

// Always store these in pointers, otherwise comparisons will fail
struct CargoTypes {
	static inline const CargoType NORMAL {{Resources::MAINTENANCE_SUPPLIES, Resources::GENERIC, Resources::METAL_LIGHT, Resources::METAL_CONDUCTIVE, Resources::SEMICONDUCTORS, Resources::RARE_EARTH}};
	static inline const CargoType AMMUNITION {{Resources::MISSILES, Resources::SABOTS}};
	static inline const CargoType FUEL {{Resources::ROCKET_FUEL}};
	static inline const CargoType LIFE_SUPPORT {{Resources::LIFE_SUPPORT}};
	static inline const CargoType NUCLEAR {{Resources::NUCLEAR_FISSION, Resources::NUCLEAR_WASTE, Resources::NUCLEAR_FUSION}};
	
	static inline const CargoType* ALL[] { &NORMAL, &AMMUNITION, &FUEL, &LIFE_SUPPORT, &NUCLEAR };
	static inline constexpr const size_t size = ARRAY_LENGTH(ALL);
};

struct CargoPnt {
	uint8_t idx = 0;
	
	constexpr CargoPnt(uint8_t idx): idx(idx) {};
	constexpr CargoPnt(const CargoType* cargoType) {
		auto itr = std::find(CargoTypes::ALL, CargoTypes::ALL + ARRAY_LENGTH(CargoTypes::ALL), cargoType);
		
		if (itr != std::end(CargoTypes::ALL)) {
			idx = itr - CargoTypes::ALL;
		} else {
			throw std::invalid_argument("Invalid CargoType pointer");
		}
	}
	
	const CargoType* operator -> () const {
		return CargoTypes::ALL[idx];
	}
	
	inline operator uint8_t () const {
		return idx;
	}
};

#endif /* SRC_GALAXY_RESOURCES_HPP_ */
