/*
 * CargoComponent.hpp
 *
 *  Created on: Jan 9, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_CARGOCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_CARGOCOMPONENT_HPP_

#include <map>

#include "galaxy/Resources.hpp"
#include "galaxy/MunitionHull.hpp"
#include "utils/SmallList.hpp"

struct CargoContainer {
	uint64_t maxVolume = 0;
	uint64_t usedVolume = 0;
	uint32_t resources[Resources::size]; // In kg
	CargoTypePnt type;
	
	CargoContainer(CargoTypePnt typePnt): type(typePnt) {};
	CargoContainer(const CargoType* type): type(CargoTypePnt(type)) {};
	CargoContainer(): type(&CargoTypes::NORMAL) {};
};

struct CargoComponent {
	SmallList<CargoContainer, 4> resources[Resources::size];
	SmallList<CargoContainer, 4> types[CargoTypes::size];
	std::map<MunitionHull*, uint32_t> munitions;
	uint32_t mass = 0;
	bool cargoChanged = false;
	
	CargoComponent(const ShipHull& hull) {
		
	}
	
	CargoComponent(const ColonyComponent& colony) {
		
	}
	
//	uint32_t getCargoAmount(ResourcePnt resource) {
//		
//	}
//	
//	uint32_t getCargoAmount(MunitionHull* munition) {
//		
//	}
//	
//	uint32_t getUsedCargoVolume(ResourcePnt resource) {
//		
//	}
//	
//	uint32_t getUsedCargoVolume(MunitionHull* munition) {
//		
//	}
//	
//	uint32_t getMaxCargoVolume(ResourcePnt resource) {
//		
//	}
//	
//	uint32_t getMaxCargoVolume(MunitionHull* munition) {
//		
//	}
//	
//	uint32_t getUsedCargoMass(ResourcePnt resource) {
//		
//	}
//	
//	uint32_t getUsedCargoMass(MunitionHull* munition) {
//		
//	}
//	
//	bool addCargo(ResourcePnt resource, uint32_t amount) {
//		assert(resource->specificVolume > 0);
//		
//		SmallList<CargoContainer, 4>& list = resources[resource];
//		
//		if (!list.empty()) {
//		
//			uint32_t volumeToBeStored = amount * resource->specificVolume;
//			
//			decltype(list)::iterator cargoItr = list.begin();
//			
//			while(cargoItr->usedVolume + volumeToBeStored > cargoItr->maxVolume && cargoItr != list.end()) {
//				cargoItr++;
//			}
//			
//			if (cargoItr != list.end()) {
//				cargoItr->usedVolume += volumeToBeStored;
//				cargoItr->resources[resource] += amount;
//				
//				mass += amount;
//				cargoChanged = true;
//				return true;
//			}
//		}
//		
//		return false;
//	}
//	
//	bool addCargo(MunitionHull* munition, uint32_t amount) {
//		SmallList<CargoContainer, 4>& list = resources[munition->storageType];
//		
//		if (!list.empty()) {
//		
//			uint32_t volumeToBeStored = amount * munition->volume;
//			
//			decltype(list)::iterator cargoItr = list.begin();
//			
//			while(cargoItr->usedVolume + volumeToBeStored > cargoItr->maxVolume && cargoItr != list.end()) {
//				cargoItr++;
//			}
//			
//			if (cargoItr != list.end()) {
//				cargoItr->usedVolume += volumeToBeStored;
//				cargoItr->resources[munition->storageType] += amount;
//				
//				mass += amount;
//				cargoChanged = true;
//				return true;
//			}
//		}
//		
//		return false;
//	}
//	
//	uint32_t retrieveCargo(ResourcePnt resource, uint32_t amount) {
//		
//	}
//	
//	uint32_t retrieveCargo(MunitionHull* munition, uint32_t amount) {
//		
//	}
	
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_CARGOCOMPONENT_HPP_ */
