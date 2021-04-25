/*
 * Player.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: exuvo
 */

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


void Player::clearSelection() {
	selection.clear();
	selectionSet.clear();
}

bool Player::addSelection(EntityReference& ref) {
	if (selectionSet.insert(ref).second) {
		selection.push_back(ref);
		return true;
	}
	return false;
}

bool Player::addSelection(std::vector<EntityReference>& entities) {
	bool addedAnything = false;
	
	for (const EntityReference& ref : entities) {
		if (selectionSet.insert(ref).second) {
			selection.push_back(ref);
			addedAnything = true;
		}
	}
	
	return addedAnything;
	
}

bool Player::removeSelection(EntityReference& ref) {
	if (selectionSet.erase(ref)) {
		vectorEraseUnorderedVal(selection, ref);
		return true;
	}
	
	return false;
}

bool Player::removeSelection(std::vector<EntityReference>& entities) {
	bool removedAnything = false;
	
	for (EntityReference& ref : entities) {
		if (selectionSet.erase(ref)) {
			vectorEraseUnorderedVal(selection, ref);
			removedAnything = true;
		}
	}
	
	return removedAnything;
}

void Player::replaceSelection(EntityReference& newRef, EntityReference& oldRef) {
	std::vector<EntityReference>::iterator position = std::find(selection.begin(), selection.end(), oldRef);
	
	if (position != selection.end()) {
    *position = newRef;
    selectionSet.erase(oldRef);
	} else {
		selection.push_back(newRef);
	}
	
	selectionSet.insert(newRef);
}

void Player::replaceSelection(EntityReference& newRef, uint16_t idx) {
	selectionSet.erase(selection[idx]);
	selection[idx] = newRef;
	selectionSet.insert(newRef);
}

void Player::replaceSelection(EntityReference& newRef, std::vector<EntityReference>::iterator& it) {
	selectionSet.erase(static_cast<EntityReference&>(*it));
	*it = newRef;
	selectionSet.insert(newRef);
	it--;
}
