/*
 * PartStatesComponent.hpp
 *
 *  Created on: May 2, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_PARTSTATESCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_PARTSTATESCOMPONENT_HPP_

#include <vector>
#include <queue>
#include <deque>
#include <entt/entt.hpp>

#include "utils/quadtree/SmallList.hpp"
#include "IDComponents.hpp"

#include <Refureku/NativeProperties.h>
#include "refureku/PartStatesComponent.rfk.h"

#define PART_STATES_COUNT 8

struct PartStateIndex {
	uint8_t idx;
	
	PartStateIndex(uint8_t idx): idx(idx) {}
	
	inline uint8_t operator() () const {
		return idx;
	}
	
	inline operator uint16_t () const {
		return idx;
	}
};

struct MunitionHull;

struct FueledPartState {
	uint32_t fuelEnergyRemaining;
	uint64_t totalFuelEnergyRemaining;
};

struct PoweringPartState {
	uint32_t availablePower;
	uint32_t producedPower;
};

struct PoweredPartState {
	uint32_t requestedPower;
	uint32_t givenPower;
};

struct PassiveSensorState {
	uint64_t lastScan;
};

struct ChargedPartState {
	uint32_t charge;
	uint64_t expectedFullAt;
};

struct AmmunitionPartState {
	MunitionHull* type = nullptr;
	uint8_t amount;
	uint64_t reloadedAt; // time remaining when part is disabled
};

struct WeaponPartState {
	PartIndex<TargetingComputer> targetingComputer;
};

struct PartStatesComponent;

struct TargetingComputerStateComparator {
	TargetingComputerStateComparator(ShipHull& hull, PartStatesComponent& partStates): hull(hull), partStates(partStates) {};
	ShipHull& hull;
	PartStatesComponent& partStates;
	bool operator() (PartIndex<WeaponPart>, PartIndex<WeaponPart>) const;
};

struct TargetingComputerState {
	EntityReference target;
	uint64_t lockCompletionAt;
	SmallList<PartIndex<WeaponPart>, 16> linkedWeapons;
	SmallList<PartIndex<WeaponPart>, 16> readyWeapons;
	SmallList<PartIndex<WeaponPart>, 16> disabledWeapons;
	std::priority_queue<PartIndex<WeaponPart>, std::vector<PartIndex<WeaponPart>>, TargetingComputerStateComparator> reloadingWeapons;
	std::priority_queue<PartIndex<WeaponPart>, std::vector<PartIndex<WeaponPart>>, TargetingComputerStateComparator> chargingWeapons;
	
//	reloadingWeapons(std::priority_queue(TargetingComputerStateComparator(this), std::vector<PartIndex<WeaponPart>()))
	//TODO constructor
};

struct PartStatesComponent {
	SmallList<FueledPartState, 16> fueled;
	SmallList<PoweringPartState, 32> powering;
	SmallList<PoweredPartState, 32> powered;
	SmallList<PassiveSensorState, 8> passiveSensor;
	SmallList<ChargedPartState, 32> charged;
	SmallList<AmmunitionPartState, 16> ammunition;
	SmallList<WeaponPartState, 16> weapon;
	SmallList<TargetingComputerState, 8> targetingComputer;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_PARTSTATESCOMPONENT_HPP_ */
