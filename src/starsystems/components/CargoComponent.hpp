/*
 * CargoComponent.hpp
 *
 *  Created on: Jan 9, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_CARGOCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_CARGOCOMPONENT_HPP_

#include <map>
#include <utility>

#include "galaxy/Resources.hpp"
#include "galaxy/MunitionHull.hpp"
#include "utils/SmallList.hpp"

struct ShipHull;
struct ColonyComponent;

struct RawCargoContainer {
	uint64_t maxVolume = 0;
	uint64_t usedVolume = 0;
	uint32_t resources[];
};

// 16 bytes
template<typename Derived>
struct CargoContainer {
	uint64_t maxVolume = 0;
	uint64_t usedVolume = 0;
	
	uint32_t* getResources() { return &static_cast<Derived*>(this)->resources; };  // In kg
	size_t getLength() const { return ARRAY_LENGTH(static_cast<Derived*>(this)->resources); };
	
	inline RawCargoContainer* operator *() { return reinterpret_cast<RawCargoContainer*>(this); }
};

// 48 bytes
struct OreCargoContainer : public CargoContainer<OreCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::ORE_)];
};

// 40 bytes
struct RefinedCargoContainer : public CargoContainer<RefinedCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::REFINED_)];
};

// 8 bytes
struct GoodsCargoContainer : public CargoContainer<GoodsCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::GOODS_)];
};

// 72 bytes
struct NormalCargoContainer : public CargoContainer<NormalCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::GENERIC_)];
};

// 8 bytes
struct AmmunitionCargoContainer : public CargoContainer<AmmunitionCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::AMMUNITION_)];
};

// 4 bytes
struct FuelCargoContainer : public CargoContainer<FuelCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::FUEL_)];
};

// 4 bytes
struct LifeSupportCargoContainer : public CargoContainer<LifeSupportCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::LIFE_SUPPORT_)];
};

// 12 bytes
struct NuclearCargoContainer : public CargoContainer<NuclearCargoContainer> {
	uint32_t resources[ARRAY_LENGTH(CargoTypes::NUCLEAR_)];
};

struct CargoComponent {
	OreCargoContainer ore;
	RefinedCargoContainer refined;
	GoodsCargoContainer goods;
	NormalCargoContainer generic;
	AmmunitionCargoContainer ammunition;
	FuelCargoContainer fuel;
	LifeSupportCargoContainer lifeSupport;
	NuclearCargoContainer nuclear;
	
	std::map<MunitionHull*, uint32_t> munitions;
	uint32_t mass = 0;
	bool cargoChanged = false;
	
	CargoComponent(const ShipHull& hull);
	CargoComponent(const ColonyComponent& colony);
	
	uint32_t addCargo(const Resource* resource, uint32_t amount);
	uint32_t addCargo(ResourcePnt resource, uint32_t amount);
	uint32_t addCargo(MunitionHull* munition, uint32_t amount);
	
	uint32_t retrieveCargo(const Resource* resource, uint32_t amount);
	uint32_t retrieveCargo(ResourcePnt resource, uint32_t amount);
	uint32_t retrieveCargo(MunitionHull* munition, uint32_t amount);
	
	uint32_t getCargoAmount(ResourcePnt resource);
	uint32_t getCargoAmount(MunitionHull* munition);
	
	uint32_t getUsedCargoVolume(ResourcePnt resource);
	uint32_t getUsedCargoVolume(MunitionHull* munition);
	
	uint32_t getMaxCargoVolume(ResourcePnt resource);
	uint32_t getMaxCargoVolume(MunitionHull* munition);
	
	uint32_t getUsedCargoMass(MunitionHull* munition);
	
private:
	SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> getContainerList(const Resource* resource);
	uint32_t addCargo(SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> containers, const Resource* resource, uint32_t amount);
	uint32_t retrieveCargo(SmallList<std::pair<RawCargoContainer*, uint8_t>, 2> containers, const Resource* resource, uint32_t amount);
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_CARGOCOMPONENT_HPP_ */
