/*
 * IDComponents.hpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_

#include "entt/entt.hpp"

#include "starsystems/StarSystem.hpp"

class EntityUUID {
	public:
		uint8_t starSystemID;
		uint8_t empireID;
		uint32_t entityUID;
};

class EntityReference {
	public:
		StarSystem* system;
		entt::id_type entityID;
		EntityUUID entityUUID;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_ */
