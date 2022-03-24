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
	const std::string_view name;
	const std::string_view symbol;
	
	constexpr Resource(const char* name, int specificVolume): specificVolume(specificVolume), name(name), symbol("") {};
	constexpr Resource(const char* name, const char* symbol, int specificVolume): specificVolume(specificVolume), name(name), symbol(symbol) {};
	constexpr Resource(const char* name, double density): specificVolume(densityToVolume(density)), name(name), symbol("") {};
	constexpr Resource(const char* name, const char* symbol, double density): specificVolume(densityToVolume(density)), name(name), symbol(symbol) {};
	
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
	static inline constexpr Resource IRON { "Iron", "Fe", 5.2 }; // Hematite (iron ore) 5.15 g/cm³
	static inline constexpr Resource ALUMINA { "Alumina", "Al", 4.0 }; // Alumina 3.97 g/cm³
	static inline constexpr Resource TITANIUM_OXIDE { "Titanium Oxide", "Ti", 3.9 }; // Titanium (anatase) 3.90 g/cm³
	static inline constexpr Resource SILICA { "Silica", "Si", 1.5}; // Silica (sand) 1.54 g/cm³, Silica (pure) 2.32 g/cm³
	static inline constexpr Resource COPPER { "Copper", "Cu", 8.9 }; // Copper 8.94 g/cm³, Gold 19 g/cm³
	static inline constexpr Resource RARE_EARTH_METALS { "Rare Earth Metals", "La", 7.0 }; // Neodymium 7.0 g/cm³. https://en.wikipedia.org/wiki/Rare-earth_element, https://en.wikipedia.org/wiki/Lanthanide
	static inline constexpr Resource LITHIUM_CARBONATE { "Lithium Carbonate", "Li", 2.1 }; // Lithium carbonate 2.11 g/cm³
	static inline constexpr Resource SULFUR { "Sulfur", "S", 2.1 }; // Sulfur solid 2.07 g/cm³
	static inline constexpr Resource COAL { "Coal", "C", 1.4 }; // Coal 1.4 g/cm³
	static inline constexpr Resource OIL { "Crude Oil", "CH", 0.9 }; // Medium crude oil 0.90 g/cm³
	// Refined
	static inline constexpr Resource STEEL { "Steel", 11.7 }; // Steel 11.7 g/cm³, Concrete 2.4 g/cm³, Carbonfiber, , Ceramics 4 g/cm³
	static inline constexpr Resource ALUMINIUM { "Aluminium", 2.7 }; // Aluminium 2.7 g/cm³
	static inline constexpr Resource TITANIUM { "Titanium", 4.5 }; // Titanium 4.54 g/cm³
	static inline constexpr Resource GLASS { "Glass", 2.6 }; // Glass 2.58 g/cm³
	static inline constexpr Resource SEMICONDUCTORS { "Semiconductors", 0.21 }; // Mixed Computer-related Electronics 0.21 g/cm³
	static inline constexpr Resource LITHIUM { "Lithium", 0.53 }; // Lithium 0.53 g/cm³
	static inline constexpr Resource EXPLOSIVES { "Explosives", 1.6 }; // Nitroglycering 1.6 g/cm³
	// Goods
	static inline constexpr Resource MAINTENANCE_SUPPLIES { "Spare Parts", 0.8 };
	// Munitions
	static inline constexpr Resource MISSILES { "Missiles", 0 };
	static inline constexpr Resource SABOTS { "Sabots", 0 };
	// Requires radiation shielding
	static inline constexpr Resource NUCLEAR_FISSION { "Thorium", 15.0 }; // Uranium 19.1 g/cm³, Thorium 11.7 g/cm³
	static inline constexpr Resource NUCLEAR_WASTE { "Nuclear waste", 10.0 };
	// Requires temperature control
	// Liquid deuterium (0.162 g/cm³) https://en.wikipedia.org/wiki/Deuterium#Data_for_elemental_deuterium
	static inline constexpr Resource NUCLEAR_FUSION { "Helium-3", 0.1624 }; // Deuterium-Tritium, Helium-3. See 'Fusion Reactor Fuel Modes' at http://forum.kerbalspaceprogram.com/index.php?/topic/155255-12213-kspi-extended-11414-05-7-2017-support-release-thread/
	static inline constexpr Resource ROCKET_FUEL { "Rocket Fuel", 1.5 }; // ~1.5 g/cm³, LOX + kerosene, LOX + H, nitrogen tetroxide + hydrazine. https://en.wikipedia.org/wiki/Rocket_propellant#Liquid_propellants
	// Requires temperature control
	static inline constexpr Resource FOOD { "Food", 0.79 }; // Wheat grain 0.79 g/cm³
	static inline constexpr Resource WATER { "Water", "W", 1.0 };
	
	static inline constexpr const Resource* ALL[] { &IRON, &ALUMINA, &TITANIUM_OXIDE, &SILICA, &COPPER,
	                                                &RARE_EARTH_METALS, &LITHIUM_CARBONATE, &SULFUR, &OIL, &COAL,
	                                                &STEEL, &ALUMINIUM, &TITANIUM, &GLASS, &SEMICONDUCTORS,
	                                                &LITHIUM, &EXPLOSIVES,
	                                                &MAINTENANCE_SUPPLIES, &MISSILES, &SABOTS,
	                                                &NUCLEAR_FISSION, &NUCLEAR_WASTE,
	                                                &NUCLEAR_FUSION, &ROCKET_FUEL,
	                                                &FOOD, &WATER };
	
	static inline constexpr const Resource* ALL_ORE[] {&IRON, &ALUMINA, &TITANIUM_OXIDE,
	                                                   &SILICA, &COPPER, &RARE_EARTH_METALS,
	                                                   &LITHIUM_CARBONATE, &SULFUR, &COAL, &OIL};
	
	static inline constexpr const Resource* ALL_CONSTRUCTION[] { &IRON, &ALUMINA, &TITANIUM_OXIDE, &SILICA, &COPPER, &RARE_EARTH_METALS,
	                                                             &STEEL, &ALUMINIUM, &TITANIUM, &GLASS, &SEMICONDUCTORS, &LITHIUM, &EXPLOSIVES };
	
	static inline constexpr const Resource* ALL_GOODS_UI[] { &MAINTENANCE_SUPPLIES, &FOOD, &WATER };
	static inline constexpr const Resource* ALL_FUEL_UI[] { &ROCKET_FUEL, &NUCLEAR_FUSION, &NUCLEAR_FISSION, &NUCLEAR_WASTE };
	
	static inline constexpr size_t ALL_size = ARRAY_LENGTH(ALL);
	static inline constexpr size_t ALL_ORE_size = ARRAY_LENGTH(ALL_ORE);
	static inline constexpr size_t ALL_CONSTRUCTION_size = ARRAY_LENGTH(ALL_CONSTRUCTION);
	static inline constexpr size_t ALL_GOODS_UI_size = ARRAY_LENGTH(ALL_GOODS_UI);
	static inline constexpr size_t ALL_FUEL_UI_size = ARRAY_LENGTH(ALL_FUEL_UI);
};

struct ResourcePnt {
	uint8_t idx = 0;
	
	constexpr ResourcePnt(uint8_t idx): idx(idx) {};
	constexpr ResourcePnt(const Resource* resource) {
		auto itr = std::find(Resources::ALL, Resources::ALL + Resources::ALL_size, resource);
		
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
	
	inline ResourcePnt& operator+=(uint8_t off) {this->idx += off; return *this;}
	inline ResourcePnt& operator-=(uint8_t off) {this->idx -= off; return *this;}
	friend inline ResourcePnt operator+(const ResourcePnt& x, const ResourcePnt& y) {return ResourcePnt(x.idx + y.idx);}
	friend inline uint8_t operator-(const ResourcePnt& x, const ResourcePnt& y) {return x.idx - y.idx;}
	friend inline ResourcePnt operator+(const ResourcePnt& x, uint8_t off) {return ResourcePnt(x.idx + off);}
	friend inline ResourcePnt operator-(const ResourcePnt& x, uint8_t off) {return ResourcePnt(x.idx - off);}
	friend inline ResourcePnt operator+(uint8_t off, ResourcePnt rhs) {rhs.idx += off; return rhs;}
	friend inline ResourcePnt operator-(uint8_t off, ResourcePnt rhs) {rhs.idx -= off; return rhs;}
};

struct ResourceConstructionPnt {
	uint8_t idx = 0;
	
	constexpr ResourceConstructionPnt(uint8_t idx): idx(idx) {};
	constexpr ResourceConstructionPnt(const Resource* resource) {
		auto itr = std::find(Resources::ALL_CONSTRUCTION, Resources::ALL_CONSTRUCTION + Resources::ALL_CONSTRUCTION_size, resource);
		
		if (itr != std::end(Resources::ALL_CONSTRUCTION)) {
			idx = itr - Resources::ALL_CONSTRUCTION;
		} else {
			throw std::invalid_argument("Invalid resource construction pointer");
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
	static inline constexpr const Resource* ORE_[] {&Resources::IRON, &Resources::ALUMINA, &Resources::TITANIUM_OXIDE, &Resources::SILICA, &Resources::COPPER, &Resources::RARE_EARTH_METALS, &Resources::LITHIUM_CARBONATE, &Resources::SULFUR, &Resources::COAL, &Resources::OIL};
	static inline constexpr const Resource* REFINED_[] {&Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS, &Resources::LITHIUM, &Resources::EXPLOSIVES};
	static inline constexpr const Resource* GOODS_[] {&Resources::MAINTENANCE_SUPPLIES, &Resources::MISSILES, &Resources::SABOTS, &Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS, &Resources::LITHIUM, &Resources::EXPLOSIVES};
	static inline constexpr const Resource* AMMUNITION_[] {&Resources::MISSILES, &Resources::SABOTS};
	static inline constexpr const Resource* FUEL_[] {&Resources::ROCKET_FUEL};
	static inline constexpr const Resource* LIFE_SUPPORT_[] { &Resources::WATER, &Resources::FOOD};
	static inline constexpr const Resource* NUCLEAR_[] {&Resources::NUCLEAR_FISSION, &Resources::NUCLEAR_WASTE, &Resources::NUCLEAR_FUSION};
	
	static inline const CargoType ORE {{&Resources::IRON, &Resources::ALUMINA, &Resources::TITANIUM_OXIDE, &Resources::SILICA, &Resources::COPPER, &Resources::RARE_EARTH_METALS, &Resources::LITHIUM_CARBONATE, &Resources::SULFUR, &Resources::COAL, &Resources::OIL}};
	static inline const CargoType REFINED {{&Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS, &Resources::LITHIUM, &Resources::EXPLOSIVES}};
	static inline const CargoType GOODS {{&Resources::MAINTENANCE_SUPPLIES, &Resources::MISSILES, &Resources::SABOTS, &Resources::STEEL, &Resources::ALUMINIUM, &Resources::TITANIUM, &Resources::GLASS, &Resources::SEMICONDUCTORS, &Resources::LITHIUM, &Resources::EXPLOSIVES}};
	static inline const CargoType AMMUNITION {{&Resources::MISSILES, &Resources::SABOTS}};
	static inline const CargoType FUEL {{&Resources::ROCKET_FUEL}};
	static inline const CargoType LIFE_SUPPORT {{&Resources::WATER, &Resources::FOOD}};
	static inline const CargoType NUCLEAR {{&Resources::NUCLEAR_FISSION, &Resources::NUCLEAR_WASTE, &Resources::NUCLEAR_FUSION}};
	
	static inline constexpr const CargoType* ALL[] { &ORE, &REFINED, &AMMUNITION, &FUEL, &LIFE_SUPPORT, &NUCLEAR };
	static inline constexpr size_t ALL_size = ARRAY_LENGTH(ALL);
	static inline constexpr size_t ORE_size = ARRAY_LENGTH(ORE_);
	static inline constexpr size_t REFINED_size = ARRAY_LENGTH(REFINED_);
	static inline constexpr size_t GOODS_size = ARRAY_LENGTH(GOODS_);
	static inline constexpr size_t AMMUNITION_size = ARRAY_LENGTH(AMMUNITION_);
	static inline constexpr size_t FUEL_size = ARRAY_LENGTH(FUEL_);
	static inline constexpr size_t LIFE_SUPPORT_size = ARRAY_LENGTH(LIFE_SUPPORT_);
	static inline constexpr size_t NUCLEAR_size = ARRAY_LENGTH(NUCLEAR_);
};

struct CargoTypePnt {
	uint8_t idx = 0;
	
	constexpr CargoTypePnt(uint8_t idx): idx(idx) {};
	constexpr CargoTypePnt(const CargoType* cargoType) {
		auto itr = std::find(CargoTypes::ALL, CargoTypes::ALL + CargoTypes::ALL_size, cargoType);
		
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
