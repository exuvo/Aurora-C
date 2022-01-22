/*
 * IDComponents.cpp
 *
 *  Created on: Dec 30, 2020
 *      Author: exuvo
 */

#include <boost/functional/hash.hpp>
#include <starsystems/ShadowStarSystem.hpp>
#include <functional>

#include "Aurora.hpp"
#include "IDComponents.hpp"
#include "starsystems/StarSystem.hpp"

bool EntityUUID::operator==(const EntityUUID& o) const {
	return starSystemID == o.starSystemID && empireID == o.empireID && entityUID == o.entityUID;
}

size_t EntityUUID::hash(const EntityUUID& e) {
	
	std::hash<uint8_t> h8;
	std::hash<uint32_t> h32;
	
	size_t hash_result = 0;
	boost::hash_combine(hash_result, h8(e.starSystemID));
	boost::hash_combine(hash_result, h8(e.empireID));
	boost::hash_combine(hash_result, h32(e.entityUID));
	return hash_result;
}

size_t EntityUUID::hasher::operator()(const EntityUUID& e) const {
	return EntityUUID::hash(e);
}

size_t EntityReference::hasher::operator()(const EntityReference& e) const {
	
	std::hash<uint32_t> h32;
	
	size_t hash_result = 0;
	boost::hash_combine(hash_result, h32(static_cast<uint32_t>(e.system->galacticEntityID)));
	boost::hash_combine(hash_result, h32(static_cast<uint32_t>(e.entityID)));
	boost::hash_combine(hash_result, EntityUUID::hash(e.entityUUID));
	return hash_result;
}

bool EntityReference::operator==(const EntityReference& o) const {
	return system == o.system && entityID == o.entityID && entityUUID == o.entityUUID;
}

bool EntityReference::operator()() const {
	return system != nullptr && entityID != entt::null;
}

bool EntityReference::isValid(const StarSystem& starSystem) const {
	if (system != &starSystem || !starSystem.registry.valid(entityID)) {
		return false;
	}
	
	const UUIDComponent* uuid = starSystem.registry.try_get<UUIDComponent>(entityID);
	
	if (!uuid || uuid->uuid != entityUUID) {
		return false;
	}
	
	return true;
}

bool EntityReference::isValid(const ShadowStarSystem& shadow) const {
	if (system != &shadow.starSystem || !shadow.registry.valid(entityID)) {
		return false;
	}
	
	const UUIDComponent* uuid = shadow.registry.try_get<UUIDComponent>(entityID);
	
	if (!uuid || uuid->uuid != entityUUID) {
		return false;
	}
	
	return true;
}

bool EntityReference::resolveReference(const StarSystem& starSystem) {
	bool updated = false;
	
	for (StarSystem* system : Aurora.galaxy->systems) {
		auto found = system->shadow->uuids.find(entityUUID);
		
		if (found != system->shadow->uuids.end()) {
			this->system = system;
			entityID = found->second;
			updated = true;
			break;
		}
	}
	
	if (!updated) {
		return false;
	}
	
	if (system == &starSystem) {
		return isValid(starSystem); // Might still not be valid as we looked in shadow
	}
	
	return true; // Fine for shadow use
}

bool EntityReference::resolveReference(const ShadowStarSystem& shadow) {
	for (StarSystem* system : Aurora.galaxy->systems) {
		auto found = system->shadow->uuids.find(entityUUID);
		
		if (found != system->shadow->uuids.end()) {
			this->system = system;
			entityID = found->second;
			return true;
		}
	}
	
	return false;
}

std::ostream& operator<<(std::ostream& os, const EntityReference& ref) {
	return os << ref.entityID << " in " << *ref.system;
}
