#include <starsystems/ShadowStarSystem.hpp>

/*
 * SpatialPartitioningPlanetoidsSystem.cpp
 *
 *  Created on: Dec 24, 2020
 *      Author: exuvo
 */

#include <algorithm>
#include <fmt/core.h>

#include "starsystems/systems/Systems.hpp"
#include "utils/Utils.hpp"
#include "utils/quadtree/QuadTreeAABB.hpp"

bool SpatialPartitioningPlanetoidsSystem::comparator::operator() (entt::entity a, entt::entity b) const {
	SpatialPartitioningPlanetoidsComponent& partitioningA = registry.get<SpatialPartitioningPlanetoidsComponent>(a);
	SpatialPartitioningPlanetoidsComponent& partitioningB = registry.get<SpatialPartitioningPlanetoidsComponent>(b);
 
	return partitioningA.nextExpectedUpdate > partitioningB.nextExpectedUpdate;
}

SpatialPartitioningPlanetoidsSystem::SpatialPartitioningPlanetoidsSystem(StarSystem* starSystem)
: SpatialPartitioningPlanetoidsSystem::IntervalSystem(1s, starSystem),
	updateQueue(std::priority_queue(SpatialPartitioningPlanetoidsSystem::comparator(this), std::deque<entt::entity>())),
	removedQueue(std::priority_queue(SpatialPartitioningPlanetoidsSystem::comparator(this), std::deque<entt::entity>()))
{};

void SpatialPartitioningPlanetoidsSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
//	Aspect.all(CircleComponents).one(OrbitComponent::class.java, SunComponent::class.java, AsteroidComponent::class.java)
	registry.on_construct<OrbitComponent>().connect<&SpatialPartitioningPlanetoidsSystem::inserted>(this);
	registry.on_construct<SunComponent>().connect<&SpatialPartitioningPlanetoidsSystem::inserted>(this);
	registry.on_construct<AsteroidComponent>().connect<&SpatialPartitioningPlanetoidsSystem::inserted>(this);
	
	registry.on_destroy<OrbitComponent>().connect<&SpatialPartitioningPlanetoidsSystem::removed>(this);
	registry.on_destroy<SunComponent>().connect<&SpatialPartitioningPlanetoidsSystem::removed>(this);
	registry.on_destroy<AsteroidComponent>().connect<&SpatialPartitioningPlanetoidsSystem::removed>(this);
}

void SpatialPartitioningPlanetoidsSystem::inserted(entt::registry& registry, entt::entity entity) {
	addedEntites.push_back(entity);
}

void SpatialPartitioningPlanetoidsSystem::removed(entt::registry& registry, entt::entity entity) {
	removedEntites.push_back(entity);
}

void SpatialPartitioningPlanetoidsSystem::update(entt::entity entityID) {
	TimedMovementComponent movementComponent = registry.get<TimedMovementComponent>(entityID);
	MovementValues movement = movementComponent.get(galaxy.time).value;
	CircleComponent& circle = registry.get<CircleComponent>(entityID);
	
	uint64_t nextExpectedUpdate = updateNextExpectedUpdate(entityID, movement, circle);
	
	if (!registry.has<SpatialPartitioningPlanetoidsComponent>(entityID)) {
		registry.emplace<SpatialPartitioningPlanetoidsComponent>(entityID);
	}
	
	SpatialPartitioningPlanetoidsComponent& partitioning = registry.get<SpatialPartitioningPlanetoidsComponent>(entityID);
	partitioning.nextExpectedUpdate = nextExpectedUpdate;
	starSystem.changed<SpatialPartitioningPlanetoidsComponent>(entityID);
	
	if (nextExpectedUpdate > 0) {
		updateQueue.push(entityID);
	}
	
	uint64_t radius = circle.radius / SCALE;
	
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
	partitioning.elementID = tree.insert(static_cast<uint32_t>(entityID), x - radius, y - radius, x + radius, y + radius);
	starSystem.workingShadow->quadtreePlanetoidsChanged = true;
	profilerEvents.end();
}

uint64_t SpatialPartitioningPlanetoidsSystem::updateNextExpectedUpdate(entt::entity entityID, MovementValues& movement, CircleComponent circle) {
	uint64_t nextExpectedUpdate = galaxy.time;
		
	if (!movement.velocity.isZero()) {
		
		uint32_t delay = 0.1 * (100.0 * circle.radius) / movement.velocity.norm();
		
//		std::cout << "entityID " << entityID << ": delay " << delay << std::endl; 
		
		if (delay == 0) {
			delay = 1;
		}
		
		nextExpectedUpdate += delay;

	} else {
		nextExpectedUpdate = 0;
	}
	
	if (nextExpectedUpdate == 0) {
//		std::cout << "entityID " << entityID << ": nextExpectedUpdate " << nextExpectedUpdate << std::endl; 
	} else {
//		std::cout << "entityID " << entityID << ": nextExpectedUpdate " << nextExpectedUpdate - galaxy.time << std::endl;
	}
	
	return nextExpectedUpdate;
}

void SpatialPartitioningPlanetoidsSystem::update(delta_type delta) {
	ProfilerEvents& profilerEvents = starSystem.workingShadow->profilerEvents;
	
	for (entt::entity entityID : addedEntites) {
//		std::cout << "inserted " << entityID << std::endl;
		update(entityID);
	}
	addedEntites.clear();
	
	for (entt::entity entityID : removedEntites) {
//		std::cout << "removed " << entityID << std::endl;
		removedQueue.push(entityID);
		
		SpatialPartitioningPlanetoidsComponent& partitioning = registry.get<SpatialPartitioningPlanetoidsComponent>(entityID);
		tree.remove(partitioning.elementID);
		starSystem.workingShadow->quadtreePlanetoidsChanged = true;
	}
	removedEntites.clear();
	
	while(true) {
			
			if (!updateQueue.empty()) {
				
				entt::entity entityID = updateQueue.top();
				
				if (entityID == removedQueue.top()) {
					updateQueue.pop();
					removedQueue.pop();
					continue;
				}
				
				SpatialPartitioningPlanetoidsComponent& partitioning = registry.get<SpatialPartitioningPlanetoidsComponent>(entityID);
				
//				std::cout << "eval " << entityID << " " << partitioning.nextExpectedUpdate << std::endl;
				
				if (galaxy.time >= partitioning.nextExpectedUpdate) {
				
//					std::cout << "process " << entityID << " " << partitioning.nextExpectedUpdate << std::endl;
					
					updateQueue.pop();
					
					profilerEvents.start(fmt::format("update {}", entityID));
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
		starSystem.workingShadow->quadtreePlanetoidsChanged = true;
	}
	profilerEvents.end();
}

SmallList<entt::entity> SpatialPartitioningPlanetoidsSystem::query(QuadtreeAABB& quadTree, Matrix2l worldCoordinates) {
	Matrix2i scaled = (worldCoordinates / SCALE).cast<int32_t>();
	SmallList<uint32_t> entityIDs = quadTree.query(std::array<int32_t, 4>{ scaled(0, 0), scaled(0, 1), scaled(1, 0), scaled(1, 1) }, -1);
	SmallList<entt::id_type> sameTypes = static_cast<SmallList<entt::id_type>>(entityIDs);
	
	return *reinterpret_cast<SmallList<entt::entity>*>(&sameTypes);
}
