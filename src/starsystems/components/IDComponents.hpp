/*
 * IDComponents.hpp
 *
 *  Created on: 15 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_

#include <entt/entt.hpp>

#include <Refureku/NativeProperties.h>
#include "refureku/IDComponents.rfkh.h"

class StarSystem;
class ShadowStarSystem;

struct EntityUUID {
	uint8_t starSystemID;
	uint8_t empireID;
	uint32_t entityUID;

	bool operator==(const EntityUUID& o) const;
	
	static size_t hash(const EntityUUID& e);

	struct hasher {
		size_t operator()(const EntityUUID& e) const;
	};
};

struct EntityReference {
	StarSystem* system;
	entt::entity entityID;
	EntityUUID entityUUID;

	EntityReference(): system(nullptr), entityID(entt::null), entityUUID() {}
	
	EntityReference(StarSystem* system, entt::entity entityID, EntityUUID entityUUID)
	: system(system), entityID(entityID), entityUUID(entityUUID) {}
	
	bool operator==(const EntityReference& o) const;
	bool operator()() const;
	
	struct hasher {
		size_t operator()(const EntityReference& e) const;
	};
	
	bool isValid(const StarSystem& starSystem) const;
	bool isValid(const ShadowStarSystem& shadow) const;
	bool resolveReference(const StarSystem& starSystem);
	bool resolveReference(const ShadowStarSystem& shadow);
};

std::ostream& operator<<(std::ostream& os, const EntityReference& ref);

struct RFKStruct(kodgen::ParseAllNested) UUIDComponent {
		EntityUUID uuid;
		
		UUIDComponent_GENERATED
};

File_IDComponents_GENERATED
#endif /* SRC_STARSYSTEMS_COMPONENTS_IDCOMPONENTS_HPP_ */
