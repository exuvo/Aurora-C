/*
 * IDComponents.hpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_

#include "entt/entt.hpp"

class StarSystem;

struct EntityUUID {
		uint8_t starSystemID;
		uint8_t empireID;
		uint32_t entityUID;
		
		bool operator==(const EntityUUID& o) const {
			return starSystemID == o.starSystemID && empireID == o.empireID && entityUID == o.entityUID;
		}
};

struct EntityReference {
		EntityReference(StarSystem* system, entt::entity entityID, EntityUUID entityUUID) {
			this->system = system;
			this->entityID = entt::registry::entity(entityID);
			this->entityUUID = entityUUID;
		}
		
		StarSystem* system;
		entt::entity entityID;
		EntityUUID entityUUID;
		
		bool operator==(const EntityReference& o) const {
			return system == o.system && entityID == o.entityID && entityUUID == o.entityUUID;
		}
		
		struct hasher {
			size_t operator()(const EntityReference& e) const;
		};
};

struct UUIDComponent {
		EntityUUID uuid;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_ */
