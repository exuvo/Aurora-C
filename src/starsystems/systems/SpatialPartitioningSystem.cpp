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

void SpatialPartitioningSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
}

void SpatialPartitioningSystem::update(delta_type delta) {
	
}

SmallList<entt::entity> SpatialPartitioningSystem::query(QuadtreePoint& quadTree, Matrix2l worldCoordinates) {
//	Matrix2i scaled = (worldCoordinates / SCALE + MAX/2).cast<int32_t>();
	Matrix2i scaled = (worldCoordinates / SCALE).cast<int32_t>();
	SmallList<uint32_t> entityIDs = quadTree.query(std::array<int32_t, 4>{ scaled(0, 0), scaled(0, 1), scaled(1, 0), scaled(1, 1) }, -1);
	SmallList<entt::id_type> sameTypes = static_cast<SmallList<entt::id_type>>(entityIDs);
	
	return *reinterpret_cast<SmallList<entt::entity>*>(&sameTypes);
	
	// Copy as there is no safe way to cast list of int to list of enum even if enum storage is same int type
//	SmallList<entt::entity> entities;
//	entities.reserve(entityIDs.size());
//	
//	for (uint32_t i = 0; i < entityIDs.size(); i++) {
//		entities.push_back(static_cast<entt::entity>(entityIDs[i]));
//	}
//	
//	return entities;
}
