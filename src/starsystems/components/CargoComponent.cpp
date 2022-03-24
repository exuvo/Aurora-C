
#include <algorithm>

#include "CargoComponent.hpp"

// Handles alignment padding
#define RES_DIFF(a, b) ((reinterpret_cast<intptr_t>(a) - reinterpret_cast<intptr_t>(b)) / (reinterpret_cast<intptr_t>(&Resources::ALUMINA) - reinterpret_cast<intptr_t>(&Resources::IRON)))

CargoComponent::CargoComponent(const ShipHull& hull) {
	// merge all of same cargo type
}

CargoComponent::CargoComponent(const ColonyComponent& colony) {
	ore.maxVolume = UINT64_MAX;
	refined.maxVolume = UINT64_MAX;
	goods.maxVolume = UINT64_MAX;
	ammunition.maxVolume = UINT64_MAX;
	fuel.maxVolume = UINT64_MAX;
	lifeSupport.maxVolume = UINT64_MAX;
	nuclear.maxVolume = UINT64_MAX;
}

uint32_t CargoComponent::addCargo(std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list, const Resource* resource, uint32_t amount) {
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
		
		if (++listItr != list.end() && listItr->first != nullptr) {
			container = listItr->first;
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
	std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(&Resources::MISSILES);
	
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
		
		if (++listItr != list.end() && listItr->first != nullptr) {
			container = listItr->first;
		} else {
			break;
		}
	}
	
	uint32_t added = amount - amountToStore;
	munitions[munition] += added;
	return added;
}

uint32_t CargoComponent::retrieveCargo(std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list, const Resource* resource, uint32_t amount) {
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
		
		if (++listItr != list.end() && listItr->first != nullptr) {
			container = listItr->first;
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
	
	decltype(munitions)::iterator found = munitions.find(munition);
	
	if (found != munitions.end()) {
	
		std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(&Resources::MISSILES);
		
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
			
			if (++listItr != list.end() && listItr->first != nullptr) {
				container = listItr->first;
			} else {
				break;
			}
		}
		
		uint32_t removed = amount - amountToRetrieve;
		found->second -= removed;
		return removed;
	}
	
	return 0;
}

uint32_t CargoComponent::getCargoAmount(ResourcePnt resource) {
	std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(*resource);
	
	uint32_t amountStored = 0;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		amountStored += container->resources[listItr->second];
		
		if (++listItr != list.end() && listItr->first != nullptr) {
			container = listItr->first;
		} else {
			break;
		}
	}
	
	return amountStored;
}

uint32_t CargoComponent::getUsedCargoVolume(ResourcePnt resource) {
	std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(*resource);
	
	uint32_t usedVolume = 0;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		usedVolume += container->usedVolume;
		
		if (++listItr != list.end() && listItr->first != nullptr) {
			container = listItr->first;
		} else {
			break;
		}
	}
	
	return usedVolume;
}

uint32_t CargoComponent::getMaxCargoVolume(ResourcePnt resource) {
	std::array<std::pair<RawCargoContainer*, uint8_t>, 2> list = getContainerList(*resource);
	
	uint32_t maxVolume = 0;

	auto listItr = list.begin();
	RawCargoContainer* container = listItr->first;
	
	while(true) {
		maxVolume += container->maxVolume;
		
		if (++listItr != list.end() && listItr->first != nullptr) {
			container = listItr->first;
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

constexpr std::array<std::pair<RawCargoContainer*, uint8_t>, 2> CargoComponent::getContainerList(const Resource* resource) {
#ifndef NDEBUG
	{
		RawCargoContainer a; intptr_t A = (intptr_t) a.resources - (intptr_t) &a;
		OreCargoContainer b;
		RefinedCargoContainer c;
		GoodsCargoContainer d;
		AmmunitionCargoContainer e;
		FuelCargoContainer f;
		LifeSupportCargoContainer g;
		NuclearCargoContainer h;
		assert(A == (intptr_t) b.resources - (intptr_t) &b);
		assert(A == (intptr_t) c.resources - (intptr_t) &c);
		assert(A == (intptr_t) d.resources - (intptr_t) &d);
		assert(A == (intptr_t) e.resources - (intptr_t) &e);
		assert(A == (intptr_t) f.resources - (intptr_t) &f);
		assert(A == (intptr_t) g.resources - (intptr_t) &g);
		assert(A == (intptr_t) h.resources - (intptr_t) &h);
		
		intptr_t iron = reinterpret_cast<intptr_t>(&Resources::IRON);
		intptr_t alu = reinterpret_cast<intptr_t>(&Resources::ALUMINA);
//		printf("%lu %lu %lu %lu \n", iron, alu, alu - iron, sizeof(Resource));
//		printf("%p %p %lu \n", &Resources::IRON, &Resources::ALUMINA, &Resources::ALUMINA - &Resources::IRON);
		assert(RES_DIFF(&Resources::ALUMINA, &Resources::IRON) == 1);
//		assert((alu - iron) == sizeof(Resource));
//		assert(&Resources::ALUMINA - &Resources::IRON == 1); // fails due to alignment padding if Resource size is not a power of 2
	}
#endif
	
	// Ores
	if (resource <= &Resources::OIL) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *ore, RES_DIFF(resource, &Resources::IRON) },
		        std::pair<RawCargoContainer*, uint8_t>{ nullptr, 0 }};
	}
	
	// Refined
	if (resource <= &Resources::EXPLOSIVES) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *refined, RES_DIFF(resource, &Resources::STEEL) },
		        std::pair<RawCargoContainer*, uint8_t>{ *goods, RES_DIFF(resource, &Resources::STEEL) 
			+ std::find(CargoTypes::GOODS_, CargoTypes::GOODS_ + CargoTypes::GOODS_size, &Resources::STEEL)
			- CargoTypes::GOODS_ }};
	}
	
	if (resource <= &Resources::MAINTENANCE_SUPPLIES) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *goods, RES_DIFF(resource, &Resources::MAINTENANCE_SUPPLIES) },
			{ nullptr, 0 }};
	}
	
	if (resource <= &Resources::SABOTS) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *ammunition, RES_DIFF(resource, &Resources::MISSILES) },
		        std::pair<RawCargoContainer*, uint8_t>{ *goods, RES_DIFF(resource, &Resources::MISSILES)
			+ std::find(CargoTypes::GOODS_, CargoTypes::GOODS_ + CargoTypes::GOODS_size, &Resources::MISSILES)
			- CargoTypes::GOODS_ }};
	}
	
	if (resource <= &Resources::NUCLEAR_FUSION) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *nuclear, RES_DIFF(resource, &Resources::NUCLEAR_FISSION) },
		        std::pair<RawCargoContainer*, uint8_t>{ nullptr, 0 }};
	}
	
	if (resource <= &Resources::ROCKET_FUEL) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *fuel, RES_DIFF(resource, &Resources::ROCKET_FUEL)},
		        std::pair<RawCargoContainer*, uint8_t>{ nullptr, 0 }};
	}
	
	if (resource <= &Resources::WATER) {
		return {std::pair<RawCargoContainer*, uint8_t>{ *lifeSupport, RES_DIFF(resource, &Resources::FOOD)},
		        std::pair<RawCargoContainer*, uint8_t>{ nullptr, 0 }};
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
	
	if (resource > &Resources::WATER) {
		throw std::invalid_argument("Invalid resource pointer");
	}
	
	// Should never be reached
	return {std::pair<RawCargoContainer*, uint8_t>{ nullptr, 0 },
		      std::pair<RawCargoContainer*, uint8_t>{ nullptr, 0 }};
}
