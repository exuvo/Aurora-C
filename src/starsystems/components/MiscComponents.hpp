/*
 * IDComponents.hpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_

#include "entt/entt.hpp"

struct SpatialPartitioningComponent {
		uint64_t nextExpectedUpdate;
		int32_t elementID = -1;
};

struct SpatialPartitioningPlanetoidsComponent {
		uint64_t nextExpectedUpdate;
		int32_t elementID = -1;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_ */
