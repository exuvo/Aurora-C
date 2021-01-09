/*
 * ShipParts.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_SHIPPARTS_HPP_
#define SRC_GALAXY_SHIPPARTS_HPP_

#include <string>
#include <map>
#include <unordered_map>
#include <fmt/format.h>

#include "galaxy/Resources.hpp"
#include "starsystems/components/SensorComponents.hpp"
#include "utils/enum.h"

struct BeamWavelength {
	const char name[3];
	const uint32_t length; // In nm
	
	bool operator==(const BeamWavelength& o) const {
		return this == &o;
	}
};

struct BeamWavelengths {
	// http://labman.phys.utk.edu/phys222core/modules/m6/The%20EM%20spectrum.html
	static inline constexpr BeamWavelength Microwaves   { "MW", 500000 }; // 100000 nm - 25000 nm
	static inline constexpr BeamWavelength Infrared     { "IR",  13750 }; //  25000 nm -  2500 nm
	static inline constexpr BeamWavelength VisibleLight { "L",     575 }; //    750 nm -   400 nm
	static inline constexpr BeamWavelength Ultraviolet  { "UV",    200 }; //    400 nm -     1 nm
	static inline constexpr BeamWavelength Xrays        { "X",       1 }; //      1 nm -     0.001 nm
	
	static inline constexpr BeamWavelength ALL[] { Microwaves, Infrared, VisibleLight, Ultraviolet, Xrays };
};

BETTER_ENUM(PartType, uint8_t,
	Container,
	Fueled,
	FuelWaste,
	Powering,
	Powered,
	Charged,
	Heating,
	Ammunnition,
	Thrusting,
	Weapon,
	Reactor
); // Remember to increase size of "types" below when adding a new PartType

struct Part {
	std::string name;
	uint32_t designDay;
	std::map<Resource, uint64_t> cost;
	uint32_t mass; // In kg
	uint32_t volume; // In cm3
	uint16_t types;
	uint16_t crewRequirement;
	uint8_t health;
	
	void calculateCachedValues();
	
	std::string toString() const;
	bool is(PartType type) const;
	
};

bool operator &(const Part& part, PartType type);

struct ContainerPart {
	uint32_t capacity;
	CargoType cargoType;
	
	ContainerPart(Part* part, uint32_t capacity, CargoType cargoType)
	: capacity(capacity), cargoType(cargoType) {
		part->types |= PartType::Container;
	};
};

struct CargoContainerPart: public ContainerPart {
	CargoContainerPart(Part* part, uint32_t capacity): ContainerPart(part, capacity, CargoTypes::NORMAL) {};
};

struct FuelContainerPart: public ContainerPart {
	FuelContainerPart(Part* part, uint32_t capacity): ContainerPart(part, capacity, CargoTypes::FUEL) {};
};

struct LifeSupportContainerPart: public ContainerPart {
	LifeSupportContainerPart(Part* part, uint32_t capacity): ContainerPart(part, capacity, CargoTypes::LIFE_SUPPORT) {};
};

struct AmmoContainerPart: public ContainerPart {
	AmmoContainerPart(Part* part, uint32_t capacity): ContainerPart(part, capacity, CargoTypes::AMMUNITION) {};
};

struct NuclearContainerPart: public ContainerPart {
	NuclearContainerPart(Part* part, uint32_t capacity): ContainerPart(part, capacity, CargoTypes::NUCLEAR) {};
};

struct FueledPart {
	const Resource* fuel;
	uint32_t fuelConsumption; // kg per fuelTime
	uint32_t fuelTime; // seconds of full usage for each kg
	
	FueledPart(Part* part, const Resource* fuel, uint32_t fuelConsumption, uint32_t fuelTime)
	: fuel(fuel), fuelConsumption(fuelConsumption), fuelTime(fuelTime) {
		part->types |= PartType::Fueled;
	}
};

struct FuelWastePart {
	const Resource* waste;
	
	FuelWastePart(Part* part, const Resource* waste): waste(waste) {
		part->types |= PartType::FuelWaste;
	}
};

struct PoweringPart {
	uint32_t power; // W/s
	
	PoweringPart(Part* part, uint32_t power): power(power) {
		part->types |= PartType::Powering;
	}
};

struct PoweredPart {
	uint32_t powerConsumption; // W/s
	
	PoweredPart(Part* part, uint32_t powerConsumption): powerConsumption(powerConsumption) {
		part->types |= PartType::Powered;
	}
};

struct ChargedPart {
	uint64_t capacity; // Ws
	
	ChargedPart(Part* part, uint64_t capacity): capacity(capacity) {
		part->types |= PartType::Charged;
	}
};

struct HeatingPart {
	
	HeatingPart(Part* part) {
		part->types |= PartType::Heating;
	}
};

struct AmmunitionPart {
	const Resource* ammunitionType;
	uint8_t magazineSize;
	uint16_t reloadTime;
	uint8_t ammunitionSize; // In cm radius
	
	AmmunitionPart(Part* part, const Resource* ammunitionType, uint8_t magazineSize, uint16_t reloadTime, uint8_t ammunitionSize)
	: ammunitionType(ammunitionType), magazineSize(magazineSize), reloadTime(reloadTime), ammunitionSize(ammunitionSize) {
		part->types |= PartType::Ammunnition;
	}
};

struct ThrustingPart {
	uint32_t thrust; // In N
	
	ThrustingPart(Part* part, uint32_t thrust): thrust(thrust) {
		part->types |= PartType::Thrusting;
	}
};

struct WeaponPart {
	
	WeaponPart(Part* part) {
		part->types |= PartType::Weapon;
	}
};

struct Battery: Part, PoweredPart, PoweringPart, ChargedPart {
	uint8_t efficiency; // In percent
	
	Battery(uint32_t powerConsumption, uint32_t power, uint64_t capacity, uint8_t efficiency = 100)
	: PoweredPart(this, powerConsumption), PoweringPart(this, power), ChargedPart(this, capacity),
	  efficiency(efficiency) {}
};

struct SolarPanel: Part, PoweringPart {
	uint8_t efficiency; // In percent
	
	SolarPanel(uint32_t power, uint8_t efficiency = 46)
	: PoweringPart(this, power), efficiency(efficiency) {}
};

struct Reactor: PoweringPart, FueledPart {
	Reactor(Part* part, uint32_t power, const Resource* fuel, uint32_t fuelTime)
	: PoweringPart(part, power), FueledPart(part, fuel, 1, fuelTime) {
		part->types |= PartType::Reactor;
	}
};

struct FissionReactor: Part, Reactor, FuelWastePart {
	uint8_t efficiency; // Heat to energy in %
	
	FissionReactor(uint32_t power = 1000000, uint8_t efficiency = 33)
	: Reactor(this, power, &Resources::NUCLEAR_FISSION, calculateFuelTime(power, efficiency)),
	  FuelWastePart(this, &Resources::NUCLEAR_WASTE),
	  efficiency(efficiency) {}
	
	FissionReactor(uint32_t power, uint8_t efficiency, uint32_t fuelTime)
	: Reactor(this, power, &Resources::NUCLEAR_FISSION, fuelTime), FuelWastePart(this, &Resources::NUCLEAR_WASTE),
	  efficiency(efficiency) {}
	
	static uint32_t calculateFuelTime(uint32_t power, uint8_t efficiency);
};

struct FusionReactor: Part, Reactor {
	uint8_t efficiency; // Heat to energy in %
	
	FusionReactor(uint32_t power = 1000000, uint8_t efficiency = 33)
	: Reactor(this, power, &Resources::NUCLEAR_FUSION, (86400000000000L / power) * (efficiency / 100.0)),
	  efficiency(efficiency) {}
	
	FusionReactor(uint32_t power, uint8_t efficiency, uint32_t fuelTime)
	: Reactor(this, power, &Resources::NUCLEAR_FUSION, fuelTime), efficiency(efficiency) {}
};

// https://en.wikipedia.org/wiki/Electrically_powered_spacecraft_propulsion#Types
struct ElectricalThruster: Part, ThrustingPart, PoweredPart {
	ElectricalThruster(uint32_t thrust, uint32_t powerConsumption)
	: ThrustingPart(this, thrust), PoweredPart(this, powerConsumption) {}
};

struct FueledThruster: Part, ThrustingPart, FueledPart {
	FueledThruster(uint32_t thrust, uint32_t fuelConsumption, const Resource* fuel)
	: ThrustingPart(this, thrust), FueledPart(this, fuel, fuelConsumption, 1) {}
};

struct Shield: Part, ChargedPart, PoweredPart {
	uint8_t efficiency; // Percent Energy to shield HP
	
	Shield(uint64_t capacity, uint32_t powerConsumtion, uint8_t efficiency = 50)
	: ChargedPart(this, capacity), PoweredPart(this, powerConsumtion), efficiency(efficiency) {}
};

struct BeamWeapon: Part, WeaponPart, PoweredPart, ChargedPart, HeatingPart {
	uint16_t aperature; // mm diameter
	BeamWavelength* waveLength;
	uint8_t efficiency; // Percent Energy to damage
	
	BeamWeapon(uint32_t powerConsumtion, uint16_t aperature, BeamWavelength* waveLength,
	           uint64_t capacity, uint8_t efficiency = 50)
	: WeaponPart(this), PoweredPart(this, powerConsumtion), ChargedPart(this, capacity), HeatingPart(this), 
	  aperature(aperature), waveLength(waveLength), efficiency(efficiency) {}
	
	double getRadialDivergence();
	double getBeamRadiusAtDistance(uint64_t distance);
	double getBeamArea(uint64_t distance);
	uint64_t getDeliveredEnergyTo1MSquareAtDistance(uint64_t distance);
};

struct Railgun: Part, WeaponPart, PoweredPart, ChargedPart, AmmunitionPart, HeatingPart {
	uint8_t efficiency; // Percent Energy to velocity
	
	Railgun(uint32_t powerConsumtion, uint8_t ammunitionSize, uint64_t capacity, 
	        uint8_t magazineSize, uint16_t reloadTime, uint8_t efficiency = 20)
	: WeaponPart(this), PoweredPart(this, powerConsumtion), ChargedPart(this, capacity),
	  AmmunitionPart(this, &Resources::SABOTS, magazineSize, reloadTime, ammunitionSize),
	  HeatingPart(this), efficiency(efficiency) {}
};

struct MissileLauncher: Part, WeaponPart, AmmunitionPart {
	uint32_t launchForce; // Newtons
	
	MissileLauncher(uint8_t ammunitionSize, uint8_t magazineSize, uint16_t reloadTime, uint32_t launchForce)
	: WeaponPart(this), AmmunitionPart(this, &Resources::MISSILES, magazineSize, reloadTime, ammunitionSize),
	  launchForce(launchForce) {}
};

struct Warhead: Part, WeaponPart {
	uint32_t damage;
	
	Warhead(uint32_t damage): WeaponPart(this), damage(damage) {}
};

struct TargetingComputer: Part, PoweredPart {
	uint8_t maxWeapons;
	uint16_t lockingTime;
	float prediction;
	uint64_t maxRange;
	
	TargetingComputer(uint8_t maxWeapons, uint16_t lockingTime, float prediction, uint64_t maxRange, uint32_t powerConsumtion)
	: PoweredPart(this, powerConsumtion), maxWeapons(maxWeapons), lockingTime(lockingTime), prediction(prediction),
	  maxRange(maxRange) {}
};

struct PassiveSensor: Part, PoweredPart {
	Spectrum* spectrum;
	float sensitivity;
	uint8_t arcSegments;
	float distanceResolution; // km
	int8_t angleOffset;
	float accuracy; // 1 = 100%
	uint16_t refreshDelay; // seconds
	
	PassiveSensor(Spectrum* spectrum, float sensitivity, uint8_t arcSegments, float distanceResolution,
	              int8_t angleOffset, float accuracy, uint16_t refreshDelay, uint32_t powerConsumtion)
	: PoweredPart(this, powerConsumtion), spectrum(spectrum), sensitivity(sensitivity), arcSegments(arcSegments),
	  distanceResolution(distanceResolution), angleOffset(angleOffset), accuracy(accuracy), refreshDelay(refreshDelay) {}
};

std::ostream& operator<< (std::ostream& out, const Part& part);

template <>
struct fmt::formatter<Part> {
	// Parses format specifications
	constexpr auto parse(format_parse_context& ctx) {
		auto it = ctx.begin(), end = ctx.end();
		
		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");
		
		// Return an iterator past the end of the parsed range:
		return it;
	}
	
	template<typename FormatContext>
	auto format(const Part& part, FormatContext& ctx) {
		return format_to(ctx.out(), "{}", part.toString());
	}
};

#endif /* SRC_GALAXY_SHIPPARTS_HPP_ */
