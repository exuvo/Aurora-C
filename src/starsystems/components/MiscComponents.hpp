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
#include "refureku/MiscComponents.rfkh.h"

struct RFKStruct(kodgen::ParseAllNested) SpatialPartitioningComponent {
		uint64_t nextExpectedUpdate;
		int32_t elementID = -1;
		
		SpatialPartitioningComponent_GENERATED
};

struct RFKStruct(kodgen::ParseAllNested) SpatialPartitioningPlanetoidsComponent {
		uint64_t nextExpectedUpdate;
		int32_t elementID = -1;
		
		SpatialPartitioningPlanetoidsComponent_GENERATED
};

File_MiscComponents_GENERATED
#endif /* SRC_STARSYSTEMS_COMPONENTS_MISCCOMPONENTS_HPP_ */
