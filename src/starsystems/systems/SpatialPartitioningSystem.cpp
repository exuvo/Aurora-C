#include <starsystems/ShadowStarSystem.hpp>

/*
 * SpatialPartitioningSystem.cpp
 *
 *  Created on: Dec 24, 2020
 *      Author: exuvo
 */

#include <algorithm>

#include "starsystems/systems/Systems.hpp"
#include "utils/Utils.hpp"
#include "utils/quadtree/QuadTreePoint.hpp"

bool SpatialPartitioningSystem::comparator::operator() (entt::entity a, entt::entity b) const {
	SpatialPartitioningComponent& partitioningA = registry.get<SpatialPartitioningComponent>(a);
	SpatialPartitioningComponent& partitioningB = registry.get<SpatialPartitioningComponent>(b);
 
	return partitioningA.nextExpectedUpdate > partitioningB.nextExpectedUpdate;
}

SpatialPartitioningSystem::SpatialPartitioningSystem(StarSystem* starSystem)
: SpatialPartitioningSystem::IntervalSystem(1s, starSystem),
	updateQueue(std::priority_queue(SpatialPartitioningSystem::comparator(this), std::deque<entt::entity>())),
	removedQueue(std::priority_queue(SpatialPartitioningSystem::comparator(this), std::deque<entt::entity>())),
	accelerateObserver{registry, entt::collector.group<ThrustComponent, MassComponent>()}
{};

void SpatialPartitioningSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
//	Aspect.all(TimedMovementComponent).one(ShipComponent, RailgunShotComponent, LaserShotComponent, MissileComponent);
	registry.on_construct<ShipComponent>().connect<&SpatialPartitioningSystem::inserted>(this);
	registry.on_construct<RailgunShotComponent>().connect<&SpatialPartitioningSystem::inserted>(this);
	registry.on_construct<LaserShotComponent>().connect<&SpatialPartitioningSystem::inserted>(this);
	registry.on_construct<MissileComponent>().connect<&SpatialPartitioningSystem::inserted>(this);
	
	registry.on_destroy<ShipComponent>().connect<&SpatialPartitioningSystem::removed>(this);
	registry.on_destroy<RailgunShotComponent>().connect<&SpatialPartitioningSystem::removed>(this);
	registry.on_destroy<LaserShotComponent>().connect<&SpatialPartitioningSystem::removed>(this);
	registry.on_destroy<MissileComponent>().connect<&SpatialPartitioningSystem::removed>(this);
}

void SpatialPartitioningSystem::inserted(entt::registry& registry, entt::entity entity) {
	addedEntites.push_back(entity);
}

void SpatialPartitioningSystem::removed(entt::registry& registry, entt::entity entity) {
	removedEntites.push_back(entity);
}

void SpatialPartitioningSystem::update(entt::entity entityID) {
	MovementValues movement = registry.get<TimedMovementComponent>(entityID).get(galaxy.time).value;
	uint64_t nextExpectedUpdate = updateNextExpectedUpdate(entityID, movement);
	
	if (!registry.has<SpatialPartitioningComponent>(entityID)) {
		registry.emplace<SpatialPartitioningComponent>(entityID);
	}
	
	SpatialPartitioningComponent& partitioning = registry.get<SpatialPartitioningComponent>(entityID);
	partitioning.nextExpectedUpdate = nextExpectedUpdate;
	starSystem.changed<SpatialPartitioningComponent>(entityID);
	
	if (nextExpectedUpdate > 0) {
		updateQueue.push(entityID);
	}
	
	uint64_t x = movement.position.x() / SCALE; // + MAX/2
	uint64_t y = movement.position.y() / SCALE; // + MAX/2
	
//		println("insert at $x $y ${movement.getXinKM()} ${movement.getYinKM()}")
	
	ProfilerEvents& profilerEvents = starSystem.workingShadow->profilerEvents;
	
	if (partitioning.elementID != -1) {
		profilerEvents.start("remove");
		tree.remove(partitioning.elementID);
		profilerEvents.end();
	}
	
	profilerEvents.start("insert");
	partitioning.elementID = tree.insert(static_cast<uint32_t>(entityID), x, y);
	starSystem.workingShadow->quadtreeShipsChanged = true;
	profilerEvents.end();
}

uint64_t SpatialPartitioningSystem::updateNextExpectedUpdate(entt::entity entityID, MovementValues& movement) {
	uint64_t nextExpectedUpdate = galaxy.time;
		
	if (!movement.velocity.isZero()) {
	
		//TODO val distance = distance from edge of smallest quadtree square
		
////			// at^2 + vt = distance
//			val a: Double = movement.acceleration.len()
//			val b: Double = movement.velocity.len()
//			val c: Double = -0.001 * Units.AU * 1000
//
//			val t: Double
//
//			if (a == 0.0) {
//				t = -c / b
//			} else {
//				t = WeaponSystem.getPositiveRootOfQuadraticEquation(a, b, c)
//			}
//
//			println("entityID $entityID: t $t a $a b $b c $c")
//
//			nextExpectedUpdate += maxOf(1, t.toLong())
		nextExpectedUpdate += 10;

	} else if(registry.has<ThrustComponent, MassComponent>(entityID)) {
		nextExpectedUpdate += 60;

	} else {
		nextExpectedUpdate = 0;
	}
	
	if (nextExpectedUpdate == 0) {
//			println("entityID $entityID: nextExpectedUpdate $nextExpectedUpdate")
	} else {
//			println("entityID $entityID: nextExpectedUpdate +${nextExpectedUpdate - galaxy.time}")
	}
	
	return nextExpectedUpdate;
}

void SpatialPartitioningSystem::update(delta_type delta) {
	ProfilerEvents& profilerEvents = starSystem.workingShadow->profilerEvents;
	
	for (entt::entity entityID : addedEntites) {
//		std::cout << "inserted " << entityID << std::endl;
		update(entityID);
	}
	addedEntites.clear();
	
	for (entt::entity entityID : removedEntites) {
//		std::cout << "removed " << entityID << std::endl;
		removedQueue.push(entityID);
		
		SpatialPartitioningComponent& partitioning = registry.get<SpatialPartitioningComponent>(entityID);
		tree.remove(partitioning.elementID);
		starSystem.workingShadow->quadtreeShipsChanged = true;
	}
	removedEntites.clear();
	
	// Handle ships that turned their thrusters back on
	for (const auto entity: accelerateObserver) {
		SpatialPartitioningComponent* partitioning = registry.try_get<SpatialPartitioningComponent>(entity);
			
			if (partitioning != nullptr && partitioning->nextExpectedUpdate == 0) {
//				println("accelerate inserted entityID $entityID")
				update(entity);
			}
	}
	accelerateObserver.clear();
	
	while(true) {
			
			if (!updateQueue.empty()) {
				
				entt::entity entityID = updateQueue.top();
				
				if (entityID == removedQueue.top()) {
					updateQueue.pop();
					removedQueue.pop();
					continue;
				}
				
				SpatialPartitioningComponent& partitioning = registry.get<SpatialPartitioningComponent>(entityID);
				
//				println("eval $entityID ${partitioning.nextExpectedUpdate}")
				
				if (galaxy.time >= partitioning.nextExpectedUpdate) {
				
//					println("process $entityID ${partitioning.nextExpectedUpdate}")
					
					updateQueue.pop();
					
					profilerEvents.start("update $entityID");
					update(entityID);
					profilerEvents.end();
					
				} else {
					break;
				}
				
			} else {
				break;
			}
	}
	
	profilerEvents.start("cleanup");
	if (tree.cleanupFull()) {
		starSystem.workingShadow->quadtreeShipsChanged = true;
	}
	profilerEvents.end();
}

SmallList<entt::entity> SpatialPartitioningSystem::query(QuadtreePoint& quadTree, Matrix2l worldCoordinates) {
	Matrix2i scaled = (worldCoordinates / SCALE).cast<int32_t>();
	SmallList<uint32_t> entityIDs = quadTree.query(std::array<int32_t, 4>{ scaled(0, 0), scaled(0, 1), scaled(1, 0), scaled(1, 1) }, -1);
	SmallList<entt::id_type> sameTypes = static_cast<SmallList<entt::id_type>>(entityIDs);
	
	return *reinterpret_cast<SmallList<entt::entity>*>(&sameTypes);
}
