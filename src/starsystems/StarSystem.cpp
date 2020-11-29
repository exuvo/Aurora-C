/*
 * StarSystem.cpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#include <thread>
#include <iostream>

#include "Tracy.hpp"

#include "StarSystem.hpp"
#include "systems/Systems.hpp"
#include "components/Components.hpp"

template<typename Component>
void registerComponentListener(entt::registry& registry, StarSystem* starSystem) {
	registry.on_construct<Component>().template connect<&StarSystem::added<Component>>(starSystem);
	registry.on_destroy<Component>().template connect<&StarSystem::deleted<Component>>(starSystem);
}

template<typename ... Component>
void registerComponentListeners(entt::registry& registry, StarSystem* starSystem) {
//	(registerComponentListener<Component>(registry, starSystem), ...); // c++ 17
	
	// older c++ https://stackoverflow.com/a/17340003/2531250
	using expand = int[];
	(void) expand { 0, ((void) registerComponentListener<Component>(registry, starSystem), 0)... };
}

void StarSystem::init(Galaxy* galaxy) {
	StarSystem::galaxy = galaxy;
	
	scheduler.attach<MovementPreSystem>(this);
//	scheduler.attach<ShipPreSystem>(this);
//	scheduler.attach<TargetingPreSystem>(this);
//	scheduler.attach<WeaponPreSystem>(this);
//	scheduler.attach<PowerPreSystem>(this);
	
//	scheduler.attach<OrbitSystem>(this);
//	scheduler.attach<ColonySystem>(this);
//	scheduler.attach<ShipSystem>(this);
//	scheduler.attach<MovementPredictedSystem>(this);
	scheduler.attach<MovementSystem>(this);
//	scheduler.attach<SolarIrradianceSystem>(this);
//	scheduler.attach<PassiveSensorSystem>(this);
//	scheduler.attach<TargetingSystem>(this);
//	scheduler.attach<WeaponSystem>(this);
//	scheduler.attach<TimedLifeSystem>(this);
//	scheduler.attach<SpatialPartitioningSystem>(this);
//	scheduler.attach<SpatialPartitioningPlanetoidsSystem>(this);
	
	scheduler.init();
	
	registerComponentListeners<TextComponent, TintComponent, RenderComponent>(registry, this);
	
	shadow = new ShadowStarSystem(this);
	workingShadow = new ShadowStarSystem(this);
}

template<typename... Component>
void StarSystem::added(entt::registry& registry, entt::entity entity) {
	LOG4CXX_DEBUG(log, "starsystem " << name << " created entity " << entity);
}

template<typename... Component>
void StarSystem::deleted(entt::registry& registry, entt::entity entity) {
	LOG4CXX_DEBUG(log, "starsystem " << name << " deleted entity " << entity);
}

void StarSystem::update(uint32_t deltaGameTime) {
//	std::this_thread::sleep_for(50ms);
//	LOG4CXX_INFO(log, "starsystem " << name << " took " << updateTime.count() << "ns");
	LOG4CXX_INFO(log, "starsystem " << name << " update");
//	std::cout << "starsystem " << name << " (" << galacticEntityID << ")" << std::endl;
	
	ProfilerEvents &profilerEvents = workingShadow->profilerEvents;
	profilerEvents.clear();
	
	profilerEvents.start("shadow clear");
	workingShadow->added.clear();
	workingShadow->deleted.clear();
	
	if (skipClearShadowChanged) {
		skipClearShadowChanged = false;
		
	} else {
		workingShadow->changed.clear();
		for (std::vector<bool> &bitVector : workingShadow->changedComponents) {
			bitVector.clear();
		}
	}
	
	workingShadow->quadtreeShipsChanged = false;
	workingShadow->quadtreePlanetoidsChanged = false;
	profilerEvents.end();
	
	profilerEvents.start("commands");
	for (Command *command : commandQueue) {
		try {
			command->apply();
		} catch (const std::exception &e) {
			LOG4CXX_ERROR(log, "Exception running command command" << command << e.what());
		}
	}
	profilerEvents.end();
	
	profilerEvents.start("processing");
	
	if (deltaGameTime <= 100) { //  || combatSubscription.entityCount > 0
	
		for (uint32_t i = 0; i < deltaGameTime; i++) {
			profilerEvents.start("process 1");
			scheduler.update(1);
			profilerEvents.end();
		}
		
	} else {
		
		uint32_t delta = 1 + deltaGameTime / 100;
		
		for (uint32_t i = 0; i < deltaGameTime / delta; i++) {
			profilerEvents.start("process $delta");
			scheduler.update(delta);
			profilerEvents.end();
		}
		
		for (uint32_t i = 0; i < deltaGameTime - delta * (deltaGameTime / delta); i++) {
			profilerEvents.start("process 1");
			scheduler.update(1);
			profilerEvents.end();
		}
		
//			std::cout << "processing deltaGameTime  " << deltaGameTime << " = " << delta << " x " << deltaGameTime / delta << " + " << deltaGameTime - delta * (deltaGameTime / delta) << std::endl;
	}
	profilerEvents.end();
	
	profilerEvents.start("shadow update");
	workingShadow->update();
	profilerEvents.end();
}

