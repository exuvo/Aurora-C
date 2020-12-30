/*
 * Player.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: exuvo
 */

#include <boost/functional/hash.hpp>
#include <functional>

#include "Aurora.hpp"
#include "Player.hpp"
#include "utils/Math.hpp"
#include "galaxy/Galaxy.hpp"

Player* Player::current = nullptr;

void Player::increaseSpeed() {
	if (speedIndex < speedSteps.size() - 1) {
		speedIndex++;
	}
	
	requestedSpeed = signum(requestedSpeed) * duration_cast<nanoseconds>(1s).count() / speedSteps[speedIndex];
	Aurora.galaxy->updateSpeed();
}

void Player::decreaseSpeed() {
	if (speedIndex > 0) {
		speedIndex--;
	}
	
	requestedSpeed = signum(requestedSpeed) * duration_cast<nanoseconds>(1s).count() / speedSteps[speedIndex];
	Aurora.galaxy->updateSpeed();
}

void Player::pauseSpeed() {
	requestedSpeed = -requestedSpeed;
	Aurora.galaxy->updateSpeed();
}

size_t EntityReference::hasher::operator()(const EntityReference& e) const {
	
	std::hash<uint8_t> h8;
	std::hash<uint32_t> h32;
	
	size_t hash_result = 0;
	boost::hash_combine(hash_result, h32(static_cast<uint32_t>(e.system->galacticEntityID)));
	boost::hash_combine(hash_result, h32(static_cast<uint32_t>(e.entityID)));
	boost::hash_combine(hash_result, h8(e.entityUUID.starSystemID));
	boost::hash_combine(hash_result, h8(e.entityUUID.empireID));
	boost::hash_combine(hash_result, h32(e.entityUUID.entityUID));
	return hash_result;
}
