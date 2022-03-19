
#include <algorithm>

#include "CargoComponent.hpp"

CargoComponent::CargoComponent(const ShipHull& hull) {
	// merge all of same cargo type
}

CargoComponent::CargoComponent(const ColonyComponent& colony) {
	
}

uint32_t CargoComponent::addCargo(SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list, const Resource* resource, uint32_t amount) {
	uint32_t amountToStore = amount;
	uint16_t specificVolume = resource->specificVolume;
	assert(specificVolume > 0);
	
	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		uint64_t freeVolume = container->maxVolume - container->usedVolume;
		
		if (freeVolume >= specificVolume) {
			uint32_t addedAmount = std::min(amountToStore, (uint32_t) (freeVolume / specificVolume));
			uint64_t addedVolume = addedAmount * specificVolume;
			
			container->resources[listItr->second] += addedAmount;
			container->usedVolume += addedVolume;
			
			amountToStore -= addedAmount;
			
			mass += addedAmount;
			cargoChanged = true;
			
			if (amountToStore == 0) {
				break;
			}
		}
		
		if (listItr != list.end()) {
			container = (++listItr)->first;
		} else {
			break;
		}
	}
	
	return amount - amountToStore;
}

uint32_t CargoComponent::addCargo(const Resource* resource, uint32_t amount) {
	return addCargo(getContainerList(resource), resource, amount);
}

uint32_t CargoComponent::addCargo(ResourcePnt resource, uint32_t amount) {
	return addCargo(*resource, amount);
}

uint32_t CargoComponent::addCargo(MunitionHull* munition, uint32_t amount) {
	SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list;
	list.push_back(ammunition, *munition->storageType - &Resources::MISSILES);
	list.push_back(generic, 2 + *munition->storageType - &Resources::MISSILES);
	
	uint32_t amountToStore = amount;
	uint16_t munitionVolume = munition->volume;
	
	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		uint64_t freeVolume = container->maxVolume - container->usedVolume;
		
		if (freeVolume >= munitionVolume) {
			uint32_t addedAmount = std::min(amountToStore, (uint32_t) (freeVolume / munitionVolume));
			uint64_t addedVolume = addedAmount * munitionVolume;
			
			container->resources[listItr->second] += addedAmount;
			container->usedVolume += addedVolume;
			
			amountToStore -= addedAmount;
			
			mass += addedAmount * munition->loadedMass;
			cargoChanged = true;
			
			if (amountToStore == 0) {
				break;
			}
		}
		
		if (listItr != list.end()) {
			container = (++listItr)->first;
		} else {
			break;
		}
	}
	
	return amount - amountToStore;
}

uint32_t CargoComponent::retrieveCargo(SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list, const Resource* resource, uint32_t amount) {
	assert(resource->specificVolume > 0);
	
	uint32_t amountToRetrieve = amount;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		if (container->resources[listItr->second] > 0) {
			uint32_t containerAmount = container->resources[listItr->second];
			uint32_t removed = std::min(amountToRetrieve, containerAmount);
			amountToRetrieve -= removed;
			
			container->resources[listItr->second] = containerAmount - removed;
			container->usedVolume -= removed * resource->specificVolume;
			
			mass -= removed;
			cargoChanged = true;
			
			if (amountToRetrieve == 0) {
				break;
			}
		}
		
		if (listItr != list.end()) {
			container = (++listItr)->first;
		} else {
			break;
		}
	}
	
	return amount - amountToRetrieve;
}

uint32_t CargoComponent::retrieveCargo(const Resource* resource, uint32_t amount) {
	return retrieveCargo(getContainerList(resource), resource, amount);
}

uint32_t CargoComponent::retrieveCargo(ResourcePnt resource, uint32_t amount) {
	return retrieveCargo(*resource, amount);
}

uint32_t CargoComponent::retrieveCargo(MunitionHull* munition, uint32_t amount) {
	
	decltype(munitions)::const_iterator found = munitions.find(munition);
	
	if (found != munitions.end()) {
	
		SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list;
		list.push_back(ammunition, *munition->storageType - &Resources::MISSILES);
		list.push_back(generic, 2 + *munition->storageType - &Resources::MISSILES);
		
		uint32_t amountToRetrieve = std::min(amount, found->second);
		uint16_t munitionMass = munition->loadedMass;
	
		auto listItr = list.begin();
		RawCargoContainer* container = listItr->first;
		
		while(true) {
			if (container->resources[listItr->second] >= munitionMass) {
				uint32_t containerMass = container->resources[listItr->second];
				uint32_t removed = std::min(amountToRetrieve, containerMass);
				amountToRetrieve -= removed;
				
				container->resources[listItr->second] = containerMass - removed;
				container->usedVolume -= removed * munition->volume;
				
				mass -= removed * munitionMass;
				cargoChanged = true;
				
				if (amountToRetrieve == 0) {
					break;
				}
			}
			
			if (listItr != list.end()) {
				container = (++listItr)->first;
			} else {
				break;
			}
		}
		
		return amount - amountToRetrieve;
	}
	
	return 0;
}

uint32_t CargoComponent::getCargoAmount(ResourcePnt resource) {
	SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(*resource);
	
	uint32_t amountStored = 0;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		amountStored += container->resources[listItr->second];
		
		if (listItr != list.end()) {
			container = (++listItr)->first;
		} else {
			break;
		}
	}
	
	return amountStored;
}

uint32_t CargoComponent::getUsedCargoVolume(ResourcePnt resource) {
	SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(*resource);
	
	uint32_t usedVolume = 0;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		usedVolume += container->usedVolume;
		
		if (listItr != list.end()) {
			container = (++listItr)->first;
		} else {
			break;
		}
	}
	
	return usedVolume;
}

uint32_t CargoComponent::getMaxCargoVolume(ResourcePnt resource) {
	SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(*resource);
	
	uint32_t maxVolume = 0;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		maxVolume += container->maxVolume;
		
		if (listItr != list.end()) {
			container = (++listItr)->first;
		} else {
			break;
		}
	}
	
	return maxVolume;
}

uint32_t CargoComponent::getCargoAmount(MunitionHull* munition) {
	decltype(munitions)::const_iterator found = munitions.find(munition);
	
	if (found != munitions.end()) {
		return found->second;
	}
	
	return 0;
}

uint32_t CargoComponent::getUsedCargoVolume(MunitionHull* munition) {
	return getCargoAmount(munition) * munition->volume;
}

uint32_t CargoComponent::getMaxCargoVolume(MunitionHull* munition) {
	return getMaxCargoVolume(munition->storageType);
}

uint32_t CargoComponent::getUsedCargoMass(MunitionHull* munition) {
	return getCargoAmount(munition) * munition->loadedMass;
}

SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> CargoComponent::getContainerList(const Resource* resource) {
	// Ores
	if (resource <= &Resources::OIL) {
		return {std::pair<RawCargoContainer*, uint8_t>{ ore, static_cast<uint8_t>(resource - &Resources::IRON)} };
	}
	
	// Refined
	if (resource <= &Resources::EXPLOSIVES) {
		return {std::pair<RawCargoContainer*, uint8_t>{ refined, resource - &Resources::STEEL}, 
		        std::pair<RawCargoContainer*, uint8_t>{ generic, resource - &Resources::STEEL + 4}};
	}
	
	if (resource <= &Resources::PARTS) {
		return {std::pair<RawCargoContainer*, uint8_t>{ goods, resource - &Resources::MAINTENANCE_SUPPLIES},
		        std::pair<RawCargoContainer*, uint8_t>{ generic, resource - &Resources::MAINTENANCE_SUPPLIES}};
	}
	
	if (resource <= &Resources::SABOTS) {
		return {std::pair<RawCargoContainer*, uint8_t>{ ammunition, resource - &Resources::MISSILES},
		        std::pair<RawCargoContainer*, uint8_t>{ generic, resource - &Resources::MISSILES + 2}};
	}
	
	if (resource <= &Resources::ROCKET_FUEL) {
		return {std::pair<RawCargoContainer*, uint8_t>{ fuel, resource - &Resources::ROCKET_FUEL}};
	}
	
	if (resource <= &Resources::NUCLEAR_FUSION) {
		return {std::pair<RawCargoContainer*, uint8_t>{ nuclear, resource - &Resources::NUCLEAR_FISSION}};
	}
	
	if (resource <= &Resources::LIFE_SUPPORT) {
		return {std::pair<RawCargoContainer*, uint8_t>{ lifeSupport, resource - &Resources::LIFE_SUPPORT}};
	}
	
//	switch (resource) {
//		case &Resources::IRON:
//		case &Resources::ALUMINA:
//		case &Resources::TITANIUM_OXIDE:
//		case &Resources::SILICA:
//		case &Resources::COPPER:
//		case &Resources::RARE_EARTH_METALS:
//		case &Resources::LITHIUM_CARBONATE:
//		case &Resources::SULFUR:
//		case &Resources::OIL:
//			return {{&ore, resource - &Resources::IRON}};
//	}
	
	throw std::invalid_argument("Invalid resource pointer");
}
