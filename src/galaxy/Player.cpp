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

Player* Player::current = new Player("local");

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
