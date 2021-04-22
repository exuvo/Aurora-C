/*
 * MunitionHull.cpp
 *
 *  Created on: Apr 22, 2021
 *      Author: exuvo
 */

#include <limits>
#include <numbers>

#include "MunitionHull.hpp"
#include "galaxy/ShipParts.hpp"

#define lengthToDiameterRatio 4.0

void SimpleMunitionHull::calculateValues() {
	// V = πr²h, http://mathhelpforum.com/geometry/170076-how-find-cylinder-dimensions-volume-aspect-ratio.html
	double length = lengthToDiameterRatio * 2 * radius;
	double volumeD = std::numbers::pi * radius * radius * length;
	volume = volumeD;
}

void AdvancedMunitionHull::calculateValues() {
	damage = 0;
	thrust = 0;
	thrustTime = 0;
	emptyMass = 0;
	fuelMass = 0;
	thrusters.clear();
	
	for (size_t i = 0; i < parts.size(); i++) {
		const Part* part = parts[i];
		
		if (part->is(PartType::Warhead)) {
			const Warhead* warhead = static_cast<const Warhead*>(part);
			damage += warhead->damage;
		}
		
		if (part->is(PartType::Thrusting)) {
			const ThrustingPart* thruster = dynamic_cast<const ThrustingPart*>(part);
			thrusters.push_back(MunitionPartIndex<ThrustingPart>(i));
			thrust += thruster->thrust;
		}
		
		if (part->is(PartType::Container)) {
			const ContainerPart* container = dynamic_cast<const ContainerPart*>(part);
			
			if (dynamic_cast<const FuelContainerPart*>(part) != nullptr) {
				fuelMass += container->capacity / Resources::ROCKET_FUEL.specificVolume;
				
			} else if (dynamic_cast<const NuclearContainerPart*>(part) != nullptr) {
				fuelMass += container->capacity / Resources::NUCLEAR_FISSION.specificVolume;
			}
		}
		
		emptyMass += part->mass;
		volume += part->volume; //TODO add armor
	}
	
	loadedMass = emptyMass + fuelMass;
	
	// Radius in cm. V = πr²h, http://mathhelpforum.com/geometry/170076-how-find-cylinder-dimensions-volume-aspect-ratio.html
	double length = std::pow(std::pow(2.0, 2 * lengthToDiameterRatio) * volume / std::numbers::pi, 1.0 / 3);
	double radiusD = std::sqrt(volume / std::numbers::pi / length);
	radius = (uint8_t) std::ceil(radiusD) + armorLayers.size();
	
	// Surface area in cm²
	double surface = 2 * std::numbers::pi * radiusD * length + 2 * std::numbers::pi * radiusD * radiusD;
	uint32_t surfaceArea = (uint32_t) surface; //TODO add armor
//	println("length $length, diameter ${2 * radius}, surface $surface, volume ${std::numbers::pi * length * radius2 * radius2}")
	
	uint32_t fuelConsumption = 0;
	
	for (const MunitionPartIndex<ThrustingPart> thrusterIdx : thrusters) {
		const Part* part = parts[thrusterIdx];
		
		if (part->is(PartType::Fueled)) {
			const FueledPart* fueled = dynamic_cast<const FueledPart*>(part);
			
			fuelConsumption += fueled->fuelConsumption;
		}
	}
	
	if (fuelConsumption == 0) {
		thrustTime = std::numeric_limits<uint32_t>::max();
	} else {
		thrustTime = fuelMass / fuelConsumption;
	}
}
