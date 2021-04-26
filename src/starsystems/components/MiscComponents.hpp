/*
 * IDComponents.hpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_

#include "entt/entt.hpp"

#include <Refureku/NativeProperties.h>
#include "refureku/MiscComponents.rfk.h"

struct RFKStruct(ParseAllNested) SpatialPartitioningComponent {
		uint64_t nextExpectedUpdate;
		int32_t elementID = -1;
		
		SpatialPartitioningComponent_GENERATED
};

struct RFKStruct(ParseAllNested) SpatialPartitioningPlanetoidsComponent {
		uint64_t nextExpectedUpdate;
		int32_t elementID = -1;
		
		SpatialPartitioningPlanetoidsComponent_GENERATED
};

File_GENERATED
#endif /* SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_ */
