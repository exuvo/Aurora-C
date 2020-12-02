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

class StarSystem;

struct EntityUUID {
		uint8_t starSystemID;
		uint8_t empireID;
		uint32_t entityUID;
};

struct EntityReference {
		StarSystem* system;
		entt::id_type entityID;
		EntityUUID entityUUID;
};

struct UUIDComponent {
		EntityUUID uuid;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_ */
