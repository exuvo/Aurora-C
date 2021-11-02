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
	
	scheduler.attach<OrbitSystem>(this);
//	scheduler.attach<ColonySystem>(this);
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
	
	entt::entity e3 = createEnttiy(gaia);
	registry.emplace<TextComponent>(e3, "Moon");
	registry.emplace<TimedMovementComponent>(e3);
	registry.emplace<RenderComponent>(e3);
	registry.emplace<TintComponent>(e3, vk2d::Colorf::BLUE());
	registry.emplace<CircleComponent>(e3, 1737100.0f);
	registry.emplace<OrbitComponent>(e3, e2, static_cast<float>(384400.0 / Units::AU), 0.2f, 0, 30);
	
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
	
	ProfilerEvents &profilerEvents = workingShadow->profilerEvents;
	profilerEvents.clear();
	
	profilerEvents.start("shadow clear");
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

std::ostream& operator<<(std::ostream& os, const StarSystem& s) {
	return os << s.name << " (" << s.galacticEntityID << ")";
}
