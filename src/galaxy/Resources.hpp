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
struct Resource {
	const uint16_t specificVolume; // cm³/kg
	
	constexpr Resource(int specificVolume) : specificVolume(specificVolume) {};
	constexpr Resource(double density) : specificVolume(densityToVolume(density)) {};
	
	bool operator==(const Resource& o) const {
		return this == &o;
	}
	
	bool operator<(const Resource& o) const {
		return this < &o;
	}
};

// Always store these in pointers, otherwise comparisons will fail. TODO private constructor?
struct Resources {
	// No storage requirements
	// Ores https://www.aqua-calc.com/page/density-table
	static inline constexpr Resource IRON { 5.2 }; // Hematite (iron ore) 5.15 g/cm³
	static inline constexpr Resource ALUMINA { 4.0 }; // Alumina 3.97 g/cm³
	static inline constexpr Resource TITANIUM_OXIDE { 3.9 }; // Titanium (anatase) 3.90 g/cm³
	static inline constexpr Resource SILICA { 1.5}; // Silica (sand) 1.54 g/cm³, Silica (pure) 2.32 g/cm³
	static inline constexpr Resource COPPER { 8.9 }; // Copper 8.94 g/cm³, Gold 19 g/cm³
	static inline constexpr Resource RARE_EARTH_METALS { 7.0 }; // Neodymium 7.0 g/cm³. https://en.wikipedia.org/wiki/Rare-earth_element
	static inline constexpr Resource LITHIUM_CARBONATE { 2.1 }; // Lithium carbonate 2.11 g/cm³
	static inline constexpr Resource SULFUR { 2.1 }; // Sulfur solid 2.07 g/cm³
	static inline constexpr Resource OIL { 0.9 }; // Medium crude oil 0.90 g/cm³
	// Refined
	static inline constexpr Resource STEEL { 11.7 }; // Steel 11.7 g/cm³, Concrete 2.4 g/cm³, Carbonfiber, , Ceramics 4 g/cm³
	static inline constexpr Resource ALUMINIUM { 2.7 }; // Aluminium 2.7 g/cm³
	static inline constexpr Resource TITANIUM { 4.5 }; // Titanium 4.54 g/cm³
	static inline constexpr Resource GLASS { 2.6 }; // Glass 2.58 g/cm³
	static inline constexpr Resource SEMICONDUCTORS { 0.21 }; // Mixed Computer-related Electronics 0.21 g/cm³
	static inline constexpr Resource LITHIUM { 0.53 }; // Lithium 0.53 g/cm³
	static inline constexpr Resource EXPLOSIVES { 1.6 }; // Nitroglycering 1.6 g/cm³
	// Goods
	static inline constexpr Resource MAINTENANCE_SUPPLIES { 0.8 };
	static inline constexpr Resource PARTS { 1 };
	static inline constexpr Resource MISSILES { 0 };
	static inline constexpr Resource SABOTS { 0 };
	// Requires radiation shielding
	static inline constexpr Resource NUCLEAR_FISSION { 15.0 }; // Uranium 19.1 g/cm³, Thorium 11.7 g/cm³
	static inline constexpr Resource NUCLEAR_WASTE { 10.0 };
	// Requires temperature control
	// Liquid deuterium (0.162 g/cm³) https://en.wikipedia.org/wiki/Deuterium#Data_for_elemental_deuterium
	static inline constexpr Resource NUCLEAR_FUSION { 0.1624 }; // Deuterium-Tritium, Helium-3. See 'Fusion Reactor Fuel Modes' at http://forum.kerbalspaceprogram.com/index.php?/topic/155255-12213-kspi-extended-11414-05-7-2017-support-release-thread/
	static inline constexpr Resource ROCKET_FUEL { 1.5 }; // ~1.5 g/cm³, LOX + kerosene, LOX + H, nitrogen tetroxide + hydrazine. https://en.wikipedia.org/wiki/Rocket_propellant#Liquid_propellants
	// Requires temperature and atmosphere control
	static inline constexpr Resource LIFE_SUPPORT { 1.0 }; // Food, Water, Air
	
	static inline constexpr const Resource* ALL[] { &IRON, &ALUMINA, &TITANIUM_OXIDE, &SILICA, &COPPER,
	                                                &RARE_EARTH_METALS, &LITHIUM_CARBONATE, &SULFUR, &OIL,
	                                                &STEEL, &ALUMINIUM, &TITANIUM, &GLASS, &SEMICONDUCTORS, 
	                                                &LITHIUM, &EXPLOSIVES,
	                                                &MAINTENANCE_SUPPLIES, &PARTS, &MISSILES, &SABOTS,
	                                                &NUCLEAR_FISSION, &NUCLEAR_WASTE,
	                                                &NUCLEAR_FUSION, &ROCKET_FUEL,
	                                                &LIFE_SUPPORT };
	
	static inline constexpr const Resource* ALL_ORES[] { &IRON, &ALUMINA, &TITANIUM_OXIDE, &SILICA, &COPPER, &RARE_EARTH_METALS };
	
	static inline constexpr const Resource* ALL_CONSTRUCTION[] { &IRON, &ALUMINA, &TITANIUM_OXIDE, &SILICA, &COPPER, &RARE_EARTH_METALS,
	                                                &STEEL, &ALUMINIUM, &TITANIUM, &GLASS, &SEMICONDUCTORS } ;
	
	static inline constexpr const size_t size = ARRAY_LENGTH(ALL);
	static inline constexpr const size_t size_ores = ARRAY_LENGTH(ALL_ORES);
	static inline constexpr const size_t size_construction = ARRAY_LENGTH(ALL_CONSTRUCTION);
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
	
	const Resource* operator * () const {
		return Resources::ALL[idx];
	}
	
	inline operator uint8_t () const {
		return idx;
	}
};

struct CargoType {
	const std::vector<ResourcePnt> resources;
	
	CargoType(const std::vector<ResourcePnt> resources) : resources(resources) {};
	
	bool operator==(const CargoType& o) const {
		return this == &o;
	}
};

// Always store these in pointers, otherwise comparisons will fail
struct CargoTypes {
	static inline const Resource* ORE_[] {&Resources::IRON, &Resources::ALUMINA, &Resources::TITANIUM_OXIDE, &Resources::SILICA, &Resources::COPPER, &Resources::RARE_EARTH_METALS};
	static inline const Resource* REFINED_[] {&Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS};
	static inline const Resource* GOODS_[] {&Resources::MAINTENANCE_SUPPLIES, &Resources::PARTS};
	static inline const Resource* GENERIC_[] {&Resources::MAINTENANCE_SUPPLIES, &Resources::PARTS, &Resources::MISSILES, &Resources::SABOTS, &Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS};
	static inline const Resource* AMMUNITION_[] {&Resources::MISSILES, &Resources::SABOTS};
	static inline const Resource* FUEL_[] {&Resources::ROCKET_FUEL};
	static inline const Resource* LIFE_SUPPORT_[] {&Resources::LIFE_SUPPORT};
	static inline const Resource* NUCLEAR_[] {&Resources::NUCLEAR_FISSION, &Resources::NUCLEAR_WASTE, &Resources::NUCLEAR_FUSION};
	
	static inline const CargoType ORE {{&Resources::IRON, &Resources::ALUMINA, &Resources::TITANIUM_OXIDE, &Resources::SILICA, &Resources::COPPER, &Resources::RARE_EARTH_METALS}};
	static inline const CargoType REFINED {{&Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS}};
	static inline const CargoType GOODS {{&Resources::MAINTENANCE_SUPPLIES, &Resources::PARTS}};
	static inline const CargoType GENERIC {{&Resources::MAINTENANCE_SUPPLIES, &Resources::PARTS, &Resources::MISSILES, &Resources::SABOTS, &Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS}};
	static inline const CargoType AMMUNITION {{&Resources::MISSILES, &Resources::SABOTS}};
	static inline const CargoType FUEL {{&Resources::ROCKET_FUEL}};
	static inline const CargoType LIFE_SUPPORT {{&Resources::LIFE_SUPPORT}};
	static inline const CargoType NUCLEAR {{&Resources::NUCLEAR_FISSION, &Resources::NUCLEAR_WASTE, &Resources::NUCLEAR_FUSION}};
	
	static inline constexpr const CargoType* ALL[] { &ORE, &REFINED, &GENERIC, &AMMUNITION, &FUEL, &LIFE_SUPPORT, &NUCLEAR };
	static inline constexpr const size_t size = ARRAY_LENGTH(ALL);
};

struct CargoTypePnt {
	uint8_t idx = 0;
	
	constexpr CargoTypePnt(uint8_t idx): idx(idx) {};
	constexpr CargoTypePnt(const CargoType* cargoType) {
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

//struct OreResourcePnt {
//	uint8_t idx = 0;
//	
//	constexpr OreResourcePnt(uint8_t idx): idx(idx) {};
//	constexpr OreResourcePnt(const Resource* resource) {
//		auto itr = std::find(CargoTypes::ORE.resources.cbegin(), CargoTypes::ORE.resources.cend(), resource);
//		
//		if (itr != CargoTypes::ORE.resources.cend()) {
//			idx = itr - CargoTypes::ORE.resources.cbegin();
//		} else {
//			throw std::invalid_argument("Invalid resource pointer");
//		}
//	}
//	
//	const Resource* operator -> () const {
//		return CargoTypes::ORE.resources[idx];
//	}
//	
//	inline operator uint8_t () const {
//		return idx;
//	}
//};

namespace std {
	template<>
	struct hash<ResourcePnt> {
		size_t operator()(const ResourcePnt& e) const {
			return e.idx;
		}
	};
	
	template<>
	struct hash<CargoTypePnt> {
		size_t operator()(const CargoTypePnt& e) const {
			return e.idx;
		}
	};
}

#endif /* SRC_GALAXY_RESOURCES_HPP_ */
