/*
 * MovementComponents.hpp
 *
 *  Created on: 23 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_MOVEMENTCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_MOVEMENTCOMPONENTS_HPP_

//#include <glm/vec2.hpp>
//#include <Eigen/Geometry>

#include "entt/entt.hpp"

#include "utils/Math.hpp"
#include "utils/enum.h"

// In N and radians
struct ThrustComponent {
		uint64_t thrust = 0;
		uint64_t maxThrust = 0;
		float thrustAngle = 0;
		bool thrusting = false;
};

struct OrbitComponent {
	entt::entity parent = entt::null;
	float a_semiMajorAxis = 1; // In AU
	float e_eccentricity = 0; // 0 = circle, 0 < elliptic < 1
	int16_t w_argumentOfPeriapsis = 0; // In 360 degrees
	int16_t M_meanAnomaly = 0; // In 360 degrees
};

struct OnPredictedMovementComponent {};

BETTER_ENUM(ApproachType, uint8_t,
	COAST,
	BRACHISTOCHRONE, // Arrive at target using a Brachistochrone trajectory
	BALLISTIC // Arrive at target as quickly as possible
);

struct MoveToEntityComponent {
		entt::entity target;
		ApproachType approach;
};

struct MoveToPositionComponent {
		Vector2l target;
		ApproachType approach;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_MOVEMENTCOMPONENTS_HPP_ */
