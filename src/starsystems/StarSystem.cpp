#include <starsystems/ShadowStarSystem.hpp>

/*
 * StarSystem.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include <thread>
#include <iostream>

#include "Tracy.hpp"

#include "starsystems/StarSystem.hpp"
#include "starsystems/systems/Systems.hpp"
#include "galaxy/Empire.hpp"
#include "utils/Math.hpp"
#include "utils/Format.hpp"
#include "utils/Utils.hpp"

#include "galaxy/ShipHull.hpp"
#include "galaxy/ShipParts.hpp"

#undef PROFILE
#undef PROFILE_End
#define PROFILE(x) if (workingShadow->profiling) workingShadow->profilerEvents.start((x));
#define PROFILE2(x, ...) \
if (workingShadow->profiling) { \
	strBuf.clear(); \
	fmt::format_to(std::back_inserter(strBuf), (x), ##__VA_ARGS__); \
	workingShadow->profilerEvents.start(strBuf.data(), strBuf.size()); \
};
#define PROFILE_End() if (workingShadow->profiling) workingShadow->profilerEvents.end();

thread_local StarSystem* StarSystem::current = nullptr;

template<typename Component>
void registerComponentListener(entt::registry& registry, StarSystem* starSystem) {
	registry.on_construct<Component>().template connect<&StarSystem::added<Component>>(starSystem);
	registry.on_destroy<Component>().template connect<&StarSystem::deleted<Component>>(starSystem);
}

template<typename ... Component>
void registerComponentListeners(entt::registry& registry, StarSystem* starSystem) {
	(registerComponentListener<Component>(registry, starSystem), ...); // c++ 17
}

void StarSystem::init(Galaxy* galaxy) {
	StarSystem::galaxy = galaxy;
	StarSystem::current = this;
	
	systems = new Systems();
	
	systems->movementPreSystem = scheduler.attach<MovementPreSystem>(this);
//	scheduler.attach<ShipPreSystem>(this);
//	scheduler.attach<TargetingPreSystem>(this);
//	scheduler.attach<WeaponPreSystem>(this);
//	scheduler.attach<PowerPreSystem>(this);
	
	systems->orbitSystem = scheduler.attach<OrbitSystem>(this);
	systems->colonySystem = scheduler.attach<ColonySystem>(this);
//	scheduler.attach<ShipSystem>(this);
//	scheduler.attach<MovementPredictedSystem>(this);
	systems->movementSystem = scheduler.attach<MovementSystem>(this);
//	scheduler.attach<SolarIrradianceSystem>(this);
//	scheduler.attach<PassiveSensorSystem>(this);
//	scheduler.attach<TargetingSystem>(this);
	systems->weaponSystem = scheduler.attach<WeaponSystem>(this);
//	scheduler.attach<TimedLifeSystem>(this);
	systems->spatialPartitioningSystem = scheduler.attach<SpatialPartitioningSystem>(this);
	systems->spatialPartitioningPlanetoidsSystem = scheduler.attach<SpatialPartitioningPlanetoidsSystem>(this);
	
	scheduler.init(&systems);
	
	registerComponentListeners<SYNCED_COMPONENTS>(registry, this);
	
	shadow = new ShadowStarSystem(*this);
	workingShadow = new ShadowStarSystem(*this);
	
	Empire& gaia = galaxy->empires[0];
	Empire& empire1 = galaxy->empires[1];
	
	PassiveSensor* sensor1 = new PassiveSensor(&Spectra::Electromagnetic, 1e-7, 14, Units::AU * 0.3, 20, 0.97, 1, 300000);
	sensor1->name = "EM 1e-4";
	sensor1->designDay = 1;
	sensor1->calculateCachedValues();
	
//	val sensor2 = PassiveSensor(800000, Spectrum.Thermal, 1e-8, 8, Units.AU * 1, 0, 0.9, 5);
//	sensor2.name = "TH 1e-10"
//	sensor2.designDay = 1
	
	ShipHull* shipHull0 = new ShipHull();
	shipHull0->name = "Test";
	shipHull0->designDay = galaxy->day + 700;
	shipHull0->hullClass = &empire1.hullClasses[0];
	shipHull0->parts.push_back(sensor1);
	
	shipHull0->calculateCachedValues();
	empire1.shipHulls.push_back(shipHull0);
	
//	val shipHull = ShipHull()
//	shipHull.name = "Elodin"
//	shipHull.icon = StrategicIcon(StrategicIconBase.LARGE, StrategicIconCenter.INTEL)
//	shipHull.designDay = galaxy.day
//	shipHull.armorLayers = 5
//	shipHull.armorBlockHP = UByteArray(5, { 100u })
//	shipHull.armorEnergyPerDamage = ShortArray(5, { 1000 })
//	shipHull.armorBlockHP[2] = 255u
//	shipHull.armorEnergyPerDamage[2] = 800.toShort()
//	shipHull.armorBlockHP[3] = 50u
//	shipHull.armorEnergyPerDamage[3] = 3000.toShort()
////		shipClass.powerScheme = PowerScheme.SOLAR_REACTOR_BATTERY
//
//	shipHull.addPart(sensor1)
//	shipHull.addPart(sensor2)
//
//	val solarPanel = SolarPanel()
//	solarPanel.name = "Solar Panel"
//	solarPanel.cost[Resource.SEMICONDUCTORS] = 300
//	shipHull.addPart(solarPanel)
//
//	val reactor = FissionReactor(5 * Units.MEGA)
//	reactor.name = "Nuclear Reactor"
//	reactor.cost[Resource.GENERIC] = 1000
//	reactor.maxHealth = 50u
//	shipHull.addPart(reactor)
////		println("Reactor fuel consumption ${reactor.fuelConsumption} kg / ${reactor.fuelTime} s")
//
//	val nuclearStorage = NuclearContainerPart(10000)
//	nuclearStorage.name = "Nuclear Cargo"
//	nuclearStorage.cost[Resource.GENERIC] = 100
//	shipHull.addPart(nuclearStorage)
//
//	val ammoStorage = AmmoContainerPart(200000000)
//	ammoStorage.name = "Munitions Cargo"
//	ammoStorage.cost[Resource.GENERIC] = 100
//	shipHull.addPart(ammoStorage)
//
//	val fuelStorage = FuelContainerPart(400000000)
//	fuelStorage.name = "Fuel Cargo"
//	fuelStorage.cost[Resource.GENERIC] = 100
//	fuelStorage.maxHealth = 30u
//	shipHull.addPart(fuelStorage)
//
//	val battery = Battery(200 * Units.KILO, 500 * Units.KILO, 80, 100 * Units.GIGA)
//	battery.name = "Battery"
//	shipHull.addPart(battery)
//
	SimpleMunitionHull* sabot = new SimpleMunitionHull();
	sabot->name = "A sabot";
	sabot->storageType = &Resources::SABOTS;
	sabot->loadedMass = 10;
	sabot->radius = 5;
	sabot->health = 2;
	sabot->damagePattern = DamagePattern::KINETIC;
	sabot->calculateValues();
	
	SimpleMunitionHull* sabot2 = new SimpleMunitionHull();
	sabot2->name = "APDS-8";
	sabot2->storageType = &Resources::SABOTS;
	sabot2->loadedMass = 20;
	sabot2->radius = 8;
	sabot2->health = 2;
	sabot2->damagePattern = DamagePattern::KINETIC;
	sabot2->calculateValues();
	
	SimpleMunitionHull* sabot3 = new SimpleMunitionHull();
	sabot3->name = "APHE-10";
	sabot3->storageType = &Resources::SABOTS;
	sabot3->loadedMass = 30;
	sabot3->radius = 10;
	sabot3->health = 2;
	sabot3->damagePattern = DamagePattern::EXPLOSIVE;
	sabot3->calculateValues();
//
	Battery* missileBattery = new Battery(10 * Units::KILO, 50 * Units::KILO, 1 * Units::GIGA, 80);
	missileBattery->cost[ResourceConstructionPnt(&Resources::LITHIUM)] = 50;
//	
	ElectricalThruster* missileIonThruster = new ElectricalThruster(290'000, 0); // 1 * Units.KILO
	FueledThruster* missileChemicalThruster = new FueledThruster(29'000'000, 0, &Resources::ROCKET_FUEL);
//	
	FuelContainerPart* missileFuelPart = new FuelContainerPart(5000L * Resources::ROCKET_FUEL.specificVolume);
	Warhead* missileWarhead = new Warhead(100000);
	
	missileBattery->calculateCachedValues();
	missileIonThruster->calculateCachedValues();
	missileChemicalThruster->calculateCachedValues();
	missileFuelPart->calculateCachedValues();
	missileWarhead->calculateCachedValues();
//	
	AdvancedMunitionHull* missile = new AdvancedMunitionHull();
	missile->name = "Sprint missile";
	missile->storageType = &Resources::MISSILES;
	missile->parts.push_back(missileBattery);
	missile->parts.push_back(missileChemicalThruster);
	missile->parts.push_back(missileFuelPart);
	missile->parts.push_back(missileWarhead);
	missile->calculateValues();
//
//	val railgun = Railgun(2 * Units.MEGA, 5, 5 * Units.MEGA, 5, 3, 20)
//	shipHull.addPart(railgun)
//	val railgunRef = shipHull[Railgun::class][0]
//	shipHull.preferredPartMunitions[railgunRef] = sabot
//
//	val missileLauncher = MissileLauncher(7, 3, 10, 1000 * 5500)
//	missileLauncher.maxHealth = 15u
//	shipHull.addPart(missileLauncher)
//	val missileLauncherRef = shipHull[MissileLauncher::class][0]
//	shipHull.preferredPartMunitions[missileLauncherRef] = missile
//
//	val beam = BeamWeapon(1 * Units.MEGA, 1.0, BeamWavelength.Infrared, 10 * Units.MEGA)
//	shipHull.addPart(beam)
//
//	val targetingComputer1 = TargetingComputer(2, 1, 1f, (0.5 * Units.AU).toLong(),10 * Units.KILO)
//	targetingComputer1.name = "TC 05-1-2"
//	shipHull.addPart(targetingComputer1)
//	
//	val tcRef1: PartRef<TargetingComputer> = shipHull[TargetingComputer::class][0]
//	shipHull.defaultWeaponAssignments[tcRef1] = shipHull.getPartRefs().filter({ it.part is Railgun })
//	
//	val targetingComputer2 = TargetingComputer(2, 5, 1f, (2 * Units.AU).toLong(),10 * Units.KILO)
//	targetingComputer2.name = "TC 20-5-2"
//	shipHull.addPart(targetingComputer2)
//	
//	val tcRef2: PartRef<TargetingComputer> = shipHull[TargetingComputer::class][1]
//	shipHull.defaultWeaponAssignments[tcRef2] = shipHull.getPartRefs().filter({ it.part is BeamWeapon || it.part is MissileLauncher })
//
//	val ionThruster = ElectricalThruster(2000 * 982, 1 * Units.MEGA)
//	ionThruster.maxHealth = 30u
//	shipHull.addPart(ionThruster)
//
//	val chemicalThruster = FueledThruster(10000 * 982, 1)
//	chemicalThruster.maxHealth = 30u
//	shipHull.addPart(chemicalThruster)
//	
//	val shield = Shield(1 * Units.MEGA, 10 * Units.KILO, 50)
//	shield.name = "X-Booster"
//	shipHull.addPart(shield)
//	
//	val shield2 = Shield(10 * Units.KILO, 1 * Units.KILO, 80)
//	shield2.name = "S-Booster"
//	shipHull.addPart(shield2)
//	
//	shipHull.preferredCargo[Resource.NUCLEAR_FISSION] = 100
//	shipHull.preferredCargo[Resource.ROCKET_FUEL] = 10000
//	shipHull.preferredMunitions[sabot] = 100
//	shipHull.preferredMunitions[missile] = 50
//	
//	shipHull.finalize()
	
//	ShipHull shipHull2 = ShipHull(shipHull);
//	shipHull2.name = "Elodin";
//	shipHull2.designDay = galaxy.day + 700;
//	
//	shipHull2.finalize();
	
	entt::entity e1 = createEnttiy(gaia);
	registry.emplace<TextComponent>(e1, "Sun");
	registry.emplace<TimedMovementComponent>(e1).previous.value.position = {0, 0};
	registry.emplace<RenderComponent>(e1);
	registry.emplace<TintComponent>(e1, vk2d::Colorf::YELLOW());
	registry.emplace<SunComponent>(e1, 1361);
	registry.emplace<CircleComponent>(e1, 696340000.0f);
	registry.emplace<MassComponent>(e1, 1.988e30);
	
	entt::entity e2 = createEnttiy(empire1);
	registry.emplace<TextComponent>(e2, "Earth");
	registry.emplace<TimedMovementComponent>(e2);
	registry.emplace<RenderComponent>(e2);
	registry.emplace<TintComponent>(e2, vk2d::Colorf::GREY());
	registry.emplace<CircleComponent>(e2, 6371000.0f);
	registry.emplace<MassComponent>(e2, 5.972e24);
	registry.emplace<OrbitComponent>(e2, e1, 1.0f, 0.0f, -45, 0);
	ColonyComponent& earthColony = registry.emplace<ColonyComponent>(e2);
	registry.emplace<NameComponent>(e2, "Earth");
	registry.emplace<EmpireComponent>(e2, empire1);
	PlanetComponent& earth = registry.emplace<PlanetComponent>(e2);
	CargoComponent& earthCargo = registry.emplace<CargoComponent>(e2, earthColony);
	empire1.colonies.push_back(getEntityReference(e2));
	
	for (uint_fast8_t i = 0; i < 10; i++) {
		earth.oreDeposits[MiningLayer::Crust].push_back(random.next32(10000), ResourcePnt(random.next8(Resources::ALL_ORE_size - 1)));
		earth.oreDeposits[MiningLayer::Mantle].push_back(random.next32(10000), ResourcePnt(random.next8(Resources::ALL_ORE_size - 1)));
		earth.oreDeposits[MiningLayer::MoltenCore].push_back(random.next32(10000), ResourcePnt(random.next8(Resources::ALL_ORE_size - 1)));
	}
	
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::IRON);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::ALUMINA);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::TITANIUM_OXIDE);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::SILICA);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::COPPER);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::RARE_EARTH_METALS);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::LITHIUM_CARBONATE);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::SULFUR);
	earth.oreDeposits[MiningLayer::Surface].push_back(1000, &Resources::OIL);
	earth.discoveredOreDeposits[MiningLayer::Surface] = 0xFFFFFFFF;
	
	for (uint_fast8_t i = 0; i < earth.oreDeposits[MiningLayer::Surface].size(); i++) {
		OreDeposit& deposit = earth.oreDeposits[MiningLayer::Surface][i];
		earth.minableResources[MiningLayer::Surface][deposit.type] += deposit.amount;
	}
	
	// https://ourworldindata.org/land-use
	// https://en.wikipedia.org/wiki/Water_distribution_on_Earth
	earth.usableLandArea = 119'000000UL;
	earth.arableLandArea = 104'000000UL;
	earth.blockedLandArea = 28'000000UL;
	earth.freshWater =      10'621000UL * Units::CUBIC_KILOMETRE; //   10 million km³, fresh groundwater + fresh water lakes
	earth.glacierWater =    24'364000UL * Units::CUBIC_KILOMETRE; //   24 million km³, permanent snow + glaciers
	earth.seaWater =      1351'000000UL * Units::CUBIC_KILOMETRE; // 1351 million km³, sea + saline groundwater + saline lakes
	earth.pollutedWater =    1'000000UL * Units::CUBIC_KILOMETRE;
	
	earthColony.population = 10'000'000'000UL;
	
	earthColony.districtAmounts[DistrictPnt(&Districts::HousingLowDensity)] = 200;
	earthColony.districtAmounts[DistrictPnt(&Districts::HousingHighDensity)] = 100;
	
	earthColony.districtAmounts[DistrictPnt(&Districts::FarmCrops)] =     100; // 23% of agricultural land usage, 82% calorie supply, 63% protein supply
	earthColony.districtAmounts[DistrictPnt(&Districts::FarmLivestock)] = 100; // 77% of agricultural land usage, 18% calorie supply, 37% protein supply
	
	earthColony.districtAmounts[DistrictPnt(&Districts::GeneralIndustry)] = 10;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefineryBlastFurnace)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefineryArcFurnace)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefinerySmeltery)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefinerySemiconductorFab)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefineryEnricher)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefineryChemicalPlant)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefineryFuelRefinery)] = 1;
	earthColony.districtAmounts[DistrictPnt(&Districts::RefineryLithium)] = 1;
	
	earthColony.districtAmounts[DistrictPnt(&Districts::PowerSolar)] = 50;
	earthColony.districtAmounts[DistrictPnt(&Districts::PowerCoal)] = 2;
	
	earthColony.districtAmounts[DistrictPnt(&Districts::MineSurface)] = 10;
	
	earthColony.farmingLandArea = 0
		+ Districts::FarmCrops.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::FarmCrops)]
		+ Districts::FarmLivestock.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::FarmLivestock)];
	earthColony.housingLandArea = 0
		+ Districts::HousingLowDensity.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::HousingLowDensity)]
		+ Districts::HousingHighDensity.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::HousingHighDensity)];
	earthColony.miningLandArea = 0
		+ Districts::MineSurface.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::MineSurface)]
		+ Districts::MineMantle.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::MineMantle)]
		+ Districts::MineCrust.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::MineCrust)]
		+ Districts::MineMoltenCore.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::MineMoltenCore)];
	earthColony.industrialLandArea = 0
		+ Districts::GeneralIndustry.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::GeneralIndustry)]
		+ Districts::RefineryBlastFurnace.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefineryBlastFurnace)]
		+ Districts::RefineryArcFurnace.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefineryArcFurnace)]
		+ Districts::RefinerySmeltery.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefinerySmeltery)]
		+ Districts::RefinerySemiconductorFab.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefinerySemiconductorFab)]
		+ Districts::RefineryEnricher.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefineryEnricher)]
		+ Districts::RefineryChemicalPlant.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefineryChemicalPlant)]
		+ Districts::RefineryFuelRefinery.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefineryFuelRefinery)]
		+ Districts::RefineryLithium.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::RefineryLithium)];
	earthColony.powerLandArea = 0
		+ Districts::PowerSolar.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::PowerSolar)]
		+ Districts::PowerCoal.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::PowerCoal)]
		+ Districts::PowerFission.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::PowerFission)]
		+ Districts::PowerFusion.landUsage * earthColony.districtAmounts[DistrictPnt(&Districts::PowerFusion)];
	
	earthColony.shipyards.push_back(&ShipyardLocations::TERRESTIAL, &ShipyardTypes::CIVILIAN);
	earthColony.shipyards[0].slipways.push_back();
	earthColony.shipyards[0].slipways.push_back();
	earthColony.shipyards[0].modificationActivity = new ShipyardModificationAddSlipway();
	earthColony.shipyards[0].slipways[1].build(*shipHull0);
	
	earthColony.shipyards.push_back(&ShipyardLocations::ORBITAL, &ShipyardTypes::MILITARY);
	earthColony.shipyards[1].slipways.push_back();
	
	earthCargo.addCargo(&Resources::IRON, 10);
	earthCargo.addCargo(&Resources::ALUMINA, 20);
	earthCargo.addCargo(&Resources::TITANIUM_OXIDE, 30);
	earthCargo.addCargo(&Resources::SILICA, 40);
	earthCargo.addCargo(&Resources::COPPER, 50);
	earthCargo.addCargo(&Resources::RARE_EARTH_METALS, 60);
	earthCargo.addCargo(&Resources::LITHIUM_CARBONATE, 70);
	earthCargo.addCargo(&Resources::SULFUR, 80);
	earthCargo.addCargo(&Resources::OIL, 90);
	
	earthCargo.addCargo(&Resources::STEEL, 100);
	earthCargo.addCargo(&Resources::ALUMINIUM, 110);
	earthCargo.addCargo(&Resources::TITANIUM, 120);
	earthCargo.addCargo(&Resources::GLASS, 130);
	earthCargo.addCargo(&Resources::SEMICONDUCTORS, 140);
	earthCargo.addCargo(&Resources::LITHIUM, 150);
	earthCargo.addCargo(&Resources::EXPLOSIVES, 160);
	
	earthCargo.addCargo(&Resources::MAINTENANCE_SUPPLIES, 170);
	
	earthCargo.addCargo(&Resources::NUCLEAR_FISSION, 180);
	earthCargo.addCargo(&Resources::NUCLEAR_WASTE, 190);
	earthCargo.addCargo(&Resources::NUCLEAR_FUSION, 200);
	
	earthCargo.addCargo(&Resources::ROCKET_FUEL, 210);
	earthCargo.addCargo(&Resources::FOOD, 220);
	
	earthCargo.addCargo(sabot, 123);
	earthCargo.addCargo(sabot2, 72);
	earthCargo.addCargo(sabot3, 40);
	earthCargo.addCargo(missile, 100);
	
	entt::entity e3 = createEnttiy(gaia);
	registry.emplace<TextComponent>(e3, "Moon");
	registry.emplace<TimedMovementComponent>(e3);
	registry.emplace<RenderComponent>(e3);
	registry.emplace<TintComponent>(e3, vk2d::Colorf::BLUE());
	registry.emplace<CircleComponent>(e3, 1737100.0f);
	registry.emplace<OrbitComponent>(e3, e2, static_cast<float>(384400.0 / Units::AU), 0.2f, 0, 30);
	ColonyComponent& moonColony = registry.emplace<ColonyComponent>(e3, 1000, 100, 0, 50, 1000);
	registry.emplace<NameComponent>(e3, "Lunara");
	registry.emplace<EmpireComponent>(e3, empire1);
	registry.emplace<CargoComponent>(e3, moonColony);
	empire1.colonies.push_back(getEntityReference(e3));
	
	entt::entity e4 = createEnttiy(empire1);
	registry.emplace<TextComponent>(e4, "Ship");
	registry.emplace<TimedMovementComponent>(e4).previous.value.position = vectorRotateDeg(Vector2d{Units::AU * 1000 * 0.9, 0.0}, 45).cast<int64_t>();
	registry.emplace<RenderComponent>(e4);
	registry.emplace<ShipComponent>(e4);
	registry.emplace<CircleComponent>(e4, 1.0f);
	registry.emplace<MassComponent>(e4, 1000);
	registry.emplace<TintComponent>(e4, vk2d::Colorf::RED());
	
	entt::entity e5 = createEnttiy(empire1);
	registry.emplace<TextComponent>(e5, "Ship2");
	registry.emplace<TimedMovementComponent>(e5).previous.value.position = vectorRotateDeg(Vector2d{Units::AU * 1000 * 0.9, 0.0}, 10).cast<int64_t>();
	registry.emplace<RenderComponent>(e5);
	registry.emplace<ShipComponent>(e5);
	registry.emplace<CircleComponent>(e5, 1.0f);
	registry.emplace<MassComponent>(e5, 1000);
	registry.emplace<TintComponent>(e5, vk2d::Colorf::GREEN());
	
	entt::entity e6 = createEnttiy(empire1);
	registry.emplace<TextComponent>(e6, "Ship3");
	registry.emplace<TimedMovementComponent>(e6).previous.value.position = vectorRotateDeg(Vector2d{Units::AU * 1000 * 0.9, 0.0}, -180).cast<int64_t>();
	registry.emplace<RenderComponent>(e6);
	registry.emplace<ShipComponent>(e6);
	registry.emplace<CircleComponent>(e6, 1.0f);
	registry.emplace<MassComponent>(e6, 1000);
	registry.emplace<TintComponent>(e6, vk2d::Colorf::PURPLE());
}

entt::entity StarSystem::createEnttiy(Empire& empire) {
	entt::entity entity = registry.create();
	registry.emplace<UUIDComponent>(entity, EntityUUID{static_cast<uint8_t>(registry.entity(galacticEntityID)), empire.id, entityUIDCounter++});
//	history.entityCreated(entityID, world);
	return entity;
}

void StarSystem::destroyEntity(entt::entity entity) {
//	history.entityDestroyed(entityID, world);
	registry.destroy(entity);
}

EntityReference StarSystem::getEntityReference(entt::entity entity) {
	return {this, entity, registry.get<UUIDComponent>(entity).uuid};
}

template<typename Component>
void StarSystem::added(entt::registry& registry, entt::entity entity) {
	LOG4CXX_TRACE(log, "starsystem " << name << " added component " << type_name<Component>() << " to entity " << entity);
	uint32_t index = static_cast<uint32_t>(registry.entity(entity));
	workingShadow->added.reserve(index + 1);
	workingShadow->added[index] = true;
	
	if constexpr (std::is_same<Component, UUIDComponent>::value) {
		uuids[registry.get<UUIDComponent>(entity).uuid] = entity;
		workingShadow->uuidsChanged = true;
	}
}

template<typename Component>
void StarSystem::deleted(entt::registry& registry, entt::entity entity) {
	LOG4CXX_TRACE(log, "starsystem " << name << " deleted component " << type_name<Component>() << " from entity " << entity);
	uint32_t index = static_cast<uint32_t>(registry.entity(entity));
	workingShadow->deleted.reserve(index + 1);
	workingShadow->deleted[index] = true;
	
	if constexpr (std::is_same<Component, UUIDComponent>::value) {
		uuids.erase(registry.get<UUIDComponent>(entity).uuid);
		workingShadow->uuidsChanged = true;
	}
}

#define CHANGED_TEMPLATE(r, unused, component) \
template<> \
void StarSystem::changed2<component>(entt::entity entity) { \
	LOG4CXX_TRACE(log, "starsystem " << name << " changed component " << type_name<component>() << " of entity " << entity); \
	uint32_t index = static_cast<uint32_t>(registry.entity(entity)); \
	workingShadow->changed.reserve(index + 1); \
	workingShadow->changed[index] = true; \
/*	BOOST_HANA_CONSTANT_ASSERT_MSG(hana::find(syncedComponentToIndexMap, hana::type_c<component>) != hana::nothing, "missing component mapping"); */ \
	BitVector& changedVector = workingShadow->changedComponents[syncedComponentToIndexMap[hana::type_c<component>]]; \
	changedVector.reserve(index + 1); \
	changedVector[index] = true; \
};

BOOST_PP_SEQ_FOR_EACH(CHANGED_TEMPLATE, ~, SYNCED_COMPONENTS_SEQ)

void StarSystem::update(uint32_t deltaGameTime) {
	LOG4CXX_TRACE(log, "starsystem " << name << " update");
//	LOG4CXX_INFO(log, "starsystem " << name << " took " << updateTime.count() << "ns");
//	std::cout << "starsystem " << name << " (" << galacticEntityID << ")" << std::endl;
	
	ZoneScoped;
	StarSystem::current = this;
	
	workingShadow->profiling = shadow->profiling;
	workingShadow->profilerEvents.clear();
	auto strBuf = fmt::memory_buffer();
	
	if (workingShadow->profiling) {
		scheduler.profilerEvents = &workingShadow->profilerEvents;
	} else {
		scheduler.profilerEvents = nullptr;
	}
	
	PROFILE("shadow clear");
	workingShadow->added.clear();
	workingShadow->deleted.clear();
	
	if (skipClearShadowChanged) {
		skipClearShadowChanged = false;
		
	} else {
		workingShadow->changed.clear();
		for (BitVector& bitVector : workingShadow->changedComponents) {
			bitVector.clear();
		}
	}
	
	workingShadow->quadtreeShipsChanged = false;
	workingShadow->quadtreePlanetoidsChanged = false;
	PROFILE_End();
	
	PROFILE("commands");
	for (Command *command : commandQueue) {
		try {
			command->apply();
		} catch (const std::exception &e) {
			LOG4CXX_ERROR(log, "Exception running command command" << command << e.what());
		}
	}
	PROFILE_End();
	
	PROFILE("processing");
	
	if (deltaGameTime <= 100) { //  || combatSubscription.entityCount > 0
	
		for (uint32_t i = 0; i < deltaGameTime; i++) {
			PROFILE("process 1");
			scheduler.update(1);
			PROFILE_End();
		}
		
	} else {
		
		uint32_t delta = 1 + deltaGameTime / 100;
		
		for (uint32_t i = 0; i < deltaGameTime / delta; i++) {
			PROFILE2("process {}", delta);
			scheduler.update(delta);
			PROFILE_End();
		}
		
		for (uint32_t i = 0; i < deltaGameTime - delta * (deltaGameTime / delta); i++) {
			PROFILE("process 1");
			scheduler.update(1);
			PROFILE_End();
		}
		
//			std::cout << "processing deltaGameTime  " << deltaGameTime << " = " << delta << " x " << deltaGameTime / delta << " + " << deltaGameTime - delta * (deltaGameTime / delta) << std::endl;
	}
	PROFILE_End();
	
	PROFILE("shadow update");
	workingShadow->update();
	PROFILE_End();
}

bool StarSystem::operator<(const StarSystem& other) const {
	return other.name < name;
}

std::ostream& operator<<(std::ostream& os, const StarSystem& s) {
	return os << s.name << " (" << s.galacticEntityID << ")";
}
