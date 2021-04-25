/*
 * MovementSystem.cpp
 *
 *  Created on: 22 Nov 2020
 *      Author: exuvo
 */

#include <iostream>

#include "starsystems/systems/Systems.hpp"
#include "starsystems/components/Components.hpp"
#include "utils/Utils.hpp"

void MovementPreSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
}

void MovementPreSystem::update(delta_type delta) {
//	LOG4CXX_INFO(log, "update");
	
	auto view = registry.view<TimedMovementComponent, ThrustComponent, MassComponent>(entt::exclude_t<OrbitComponent, OnPredictedMovementComponent>{});
	
	for (entt::entity entity : view) {
		TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
		ThrustComponent& thrust = view.get<ThrustComponent>(entity);
		
		if (!registry.any_of<MoveToPositionComponent, MoveToEntityComponent>(entity)) {
			thrust.thrusting = !movement.get(galaxy.time).value.velocity.isZero();

		} else {
			thrust.thrusting = true;
		}
	}
}

void MovementSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	weaponSystem = systems->weaponSystem;
}

void MovementSystem::update(delta_type delta) {
//	LOG4CXX_INFO(log, "update");
	
	{
		auto view1 = registry.view<TimedMovementComponent>(entt::exclude_t<ThrustComponent, MassComponent, MoveToEntityComponent, MoveToPositionComponent, OrbitComponent, OnPredictedMovementComponent>{});
		
		for (entt::entity entity : view1) {
			TimedMovementComponent& movement = view1.get<TimedMovementComponent>(entity);
			
			if (movement.next.time != 0) {
				LOG4CXX_ERROR(log, "Entity " << entity << " on predicted movement but does not have a OnPredictedMovementComponent");
			}
			
			MovementValues shipMovementValue = movement.previous.value;
			
			auto& velocity = shipMovementValue.velocity;
			
			if (velocity.isZero()) {
				continue;
			}
			
			auto& position = shipMovementValue.position;
			auto& acceleration = shipMovementValue.acceleration;
			
			position = position + (velocity * delta) / 100;
			
			movement.previous.time = galaxy.time;
			starSystem.changed<TimedMovementComponent>(entity);
		}
	}
	
	{
		auto view2 = registry.view<TimedMovementComponent, ThrustComponent, MassComponent>(entt::exclude_t<MoveToPositionComponent, MoveToEntityComponent, OrbitComponent, OnPredictedMovementComponent>{});
		
		for (entt::entity entity : view2) {
			TimedMovementComponent& movement = view2.get<TimedMovementComponent>(entity);
			
			if (movement.next.time != 0) {
				LOG4CXX_ERROR(log, "Entity " << entity << " on predicted movement but does not have a OnPredictedMovementComponent");
			}
			
			MovementValues shipMovementValue = movement.previous.value;
			
			auto& velocity = shipMovementValue.velocity;
			
			if (velocity.isZero()) {
				continue;
			}
				
			auto& position = shipMovementValue.position;
			auto& acceleration = shipMovementValue.acceleration;
			
			MassComponent& massComponent = view2.get<MassComponent>(entity);
			ThrustComponent& thrustComponent = view2.get<ThrustComponent>(entity);
			
			const auto mass = massComponent.mass;
			const auto massL = (long) mass;
			
			const auto currentAcceleration = (100 * thrustComponent.thrust) / massL;
			const auto maxAcceleration = (100 * thrustComponent.maxThrust) / massL;
			const auto tickAcceleration = currentAcceleration * delta;
			const auto maxTickAcceleration = maxAcceleration * delta;
			const auto velocityMagnitute = velocity.norm();

			if (velocityMagnitute < (int64_t) tickAcceleration) {

				velocity = { 0,0 };
				acceleration = { 0,0 };
				
			} else {

				// Apply breaking in same direction as travel
				Vector2l tempVelocity = (velocity * -tickAcceleration ) / velocityMagnitute;
				
				if (tickAcceleration == currentAcceleration) {
					acceleration = tempVelocity;
					
				} else {
					acceleration = (velocity * -currentAcceleration) / velocityMagnitute;
				}
				
				velocity += tempVelocity;
				
				thrustComponent.thrustAngle = vectorAngle(tempVelocity);
				
				tempVelocity = velocity * delta;
				position += tempVelocity / 100;
				movement.previous.time = galaxy.time;
			}

			registry.emplace_or_replace<TextComponent>(entity, "s " + (long) velocity.norm());
			
			starSystem.changed<TimedMovementComponent, ThrustComponent, TextComponent>(entity);
		}
	}
	
	{
		auto view3 = registry.view<TimedMovementComponent, ThrustComponent, MassComponent, MoveToPositionComponent>(entt::exclude_t<OrbitComponent, OnPredictedMovementComponent>{});
		
		for (entt::entity entity : view3) {
			TimedMovementComponent& movement = view3.get<TimedMovementComponent>(entity);
			MassComponent& massComponent = view3.get<MassComponent>(entity);
			ThrustComponent& thrustComponent = view3.get<ThrustComponent>(entity);
			
			if (movement.next.time != 0) {
				LOG4CXX_ERROR(log, "Entity " << entity << " on predicted movement but does not have a OnPredictedMovementComponent");
			}
			
			MoveToPositionComponent& moveComponent = view3.get<MoveToPositionComponent>(entity);
			
			moveTo(entity, delta, movement, massComponent, thrustComponent, moveComponent.target, nullptr, entt::null, moveComponent.approach);
		}
	}
	
	{
		auto view4 = registry.view<TimedMovementComponent, ThrustComponent, MassComponent, MoveToEntityComponent>(entt::exclude_t<OrbitComponent, OnPredictedMovementComponent>{});
		
		for (entt::entity entity : view4) {
			TimedMovementComponent& movement = view4.get<TimedMovementComponent>(entity);
			MassComponent& massComponent = view4.get<MassComponent>(entity);
			ThrustComponent& thrustComponent = view4.get<ThrustComponent>(entity);
			
			if (movement.next.time != 0) {
				LOG4CXX_ERROR(log, "Entity " << entity << " on predicted movement but does not have a OnPredictedMovementComponent");
			}
			
			MoveToEntityComponent& moveComponent = view4.get<MoveToEntityComponent>(entity);
			TimedMovementComponent targetMovement = registry.get<TimedMovementComponent>(moveComponent.target);
			MovementValues targetMovementValue = targetMovement.get(galaxy.time).value;
			
			moveTo(entity, delta, movement, massComponent, thrustComponent, targetMovementValue.position, &targetMovementValue, moveComponent.target, moveComponent.approach);
		}
	}
}

void MovementSystem::moveTo(entt::entity entity, delta_type delta, TimedMovementComponent& movement, MassComponent& massComponent, ThrustComponent& thrustComponent, Vector2l targetPosition, MovementValues* targetMovement, entt::entity targetEntity, ApproachType approach) {
	
	MovementValues shipMovementValue = movement.previous.value;
	
	auto& velocity = shipMovementValue.velocity;
	auto& position = shipMovementValue.position;
	auto& acceleration = shipMovementValue.acceleration;

	const auto mass = massComponent.mass;
	const auto massL = (long) mass;
	
	const auto currentAcceleration = (100 * thrustComponent.thrust) / massL;
	const auto maxAcceleration = (100 * thrustComponent.maxThrust) / massL;
	const auto tickAcceleration = currentAcceleration * delta;
	const auto maxTickAcceleration = maxAcceleration * delta;
	
	Vector2l positionDiff = targetPosition - position;
	const auto distance = positionDiff.norm();

	//TODO if radiuses are touching we are done

	// t = sqrt(2d / a)
	// d = ut + 1/2at^2
	// Base formula = t = sqrt(2d / a), Inital speed 0 and reaches target at speed
	const auto timeToTraverseFrom0Velocity = 2 * sqrt((100 * distance) / maxTickAcceleration); 

//		println("timeToTraverseFrom0Velocity ${TimeUnits.secondsToString(timeToTraverseFrom0Velocity.toLong())}")

	if (timeToTraverseFrom0Velocity <= 1) {
		position = targetPosition;
		registry.remove_if_exists<MoveToPositionComponent>(entity);
		registry.remove_if_exists<MoveToEntityComponent>(entity);
		std::cout << "Movement: target reached distance";
		starSystem.changed<TimedMovementComponent>(entity);
		return;
	}

	const auto targetVelocity = targetMovement != nullptr ? targetMovement->velocity : Vector2l { 0,0 };
	const auto targetVelocityMagnitude = targetVelocity.norm();
	const auto angleToTarget = vectorsAngle(position, targetPosition);
//		const auto velocityAngle = velocity.angle();
	const auto velocityAngleDiffAngleToTarget = vectorsAngle(velocity, positionDiff);
//		const auto targetVelocityAngle = targetVelocity.angle()
	const auto velocityAngleScale = cos(vectorsAngle(targetVelocity, velocity));
	const auto velocityMagnitute = (uint64_t) velocity.norm();
	const auto timeToTargetWithCurrentSpeed = (100 * distance) / (velocityMagnitute + maxAcceleration);
	const auto positionDiffMagnitude = (int64_t) positionDiff.norm();
//		println("angleToTarget, $angleToTarget , velocityAngle $velocityAngle")

	switch (approach) {
		case ApproachType::BRACHISTOCHRONE: {

//		const auto relativeSpeed = targetMovement.value.velocity.cpy().sub(shipMovement.value.velocity).len() * FastMath.cos(targetMovement.value.velocity.angleToRad(shipMovement.value.velocity))
			const auto timeToStop = (velocityMagnitute - velocityAngleScale * targetVelocityMagnitude) / maxAcceleration;

//			println("timeToTargetWithCurrentSpeed ${timeToTargetWithCurrentSpeed}, timeToStop ${timeToStop}")
//				println("angleToTarget, $angleToTarget, velocityAngle $velocityAngle, tempVelocityAngle ${tempVelocity.angle()}")

//				if (timedMovement != null && (timedMovement.next == null || !timedMovement.next!!.value.position.equals(targetPosition))) {
//
//					timedMovement.setPredictionBallistic()
//				}

			Vector2l tempVelocity;
			
			if (timeToTargetWithCurrentSpeed < timeToStop && velocityMagnitute > 0) {

				if (timeToStop <= 1) {
					position = targetPosition;
					velocity = { 0,0 };
					acceleration = { 0,0 };
					
					registry.remove_if_exists<MoveToPositionComponent>(entity);
					registry.remove_if_exists<MoveToEntityComponent>(entity);
					
					std::cout << "Movement: Brachistochrone target reached time";
					registry.emplace_or_replace<TextComponent>(entity, "b " + (long) velocity.norm());
					
					starSystem.changed<TimedMovementComponent, TextComponent>(entity);
					return;
				}
				
				// Apply breaking in same direction as travel
				tempVelocity = (velocity * -tickAcceleration ) / velocityMagnitute;
				
				if (tickAcceleration == currentAcceleration) {
					acceleration = tempVelocity;
					
				} else {
					acceleration = (velocity * -currentAcceleration) / velocityMagnitute;
				}
				
				velocity += tempVelocity;
				
				thrustComponent.thrustAngle = vectorAngle(tempVelocity);
				
				registry.emplace_or_replace<TextComponent>(entity, "b " + (long) velocity.norm());

			} else {

				tempVelocity = (positionDiff* tickAcceleration) / positionDiffMagnitude;
				
				if (tickAcceleration == currentAcceleration) {
					acceleration = tempVelocity;
					
				} else {
					acceleration = (positionDiff * currentAcceleration) / positionDiffMagnitude;
				}

				if (velocityMagnitute > 10 * currentAcceleration) {
					if (abs(velocityAngleDiffAngleToTarget) <= toRadians(90)) { // Thrust slightly sideways if velocity is only somewhat in the wrong direction
						tempVelocity = vectorRotate(tempVelocity, velocityAngleDiffAngleToTarget);
//							nameMapper.get(entityID).name = "< " + velocityAngleDiffAngleToTarget

					} else { // Stop sideways velocity completly if is is too far off target
						tempVelocity = vectorRotate(tempVelocity, std::numbers::pi - velocityAngleDiffAngleToTarget);
//							nameMapper.get(entityID).name = "> " + velocityAngleDiffAngleToTarget + "," + (180 - velocityAngleDiffAngleToTarget)
					}
				}

				velocity += tempVelocity;
				
				thrustComponent.thrustAngle = (float) vectorAngle(tempVelocity);
				
				registry.emplace_or_replace<TextComponent>(entity, "a " + (long) velocity.norm());
				
//					println("angleToTarget $angleToTarget, velocityAngle $velocityAngle, angleToTargetDiffVelocityAngle $angleToTargetDiffVelocityAngle, tickAcceleration $tickAcceleration cm/s, acceleration $acceleration cm/s, tempVelocity $tempVelocity, velocity $velocity, distance $distance")
			}

			tempVelocity = velocity * delta;
			position += tempVelocity / 100;
			
			movement.previous.time = galaxy.time;

//				println("tickAcceleration $tickAcceleration cm/s, acceleration $acceleration cm/s, velocity $velocity, distance $distance")
			starSystem.changed<TimedMovementComponent, ThrustComponent, TextComponent>(entity);
			break;
		}
		case ApproachType::BALLISTIC: {

			registry.emplace_or_replace<TextComponent>(entity, "a " + velocityMagnitute);

			if (movement.next.time == 0 && targetEntity != entt::null && velocityMagnitute == 0L) {

				std::cout << "Movement: ballistic prediction to target";

				const auto timeToTarget = sqrt((2 * 100 * distance) / (double) maxAcceleration);

				const auto finalVelocity = maxAcceleration * timeToTarget;
				
				std::cout << "timeToTarget $timeToTarget, final velocity ${finalVelocity / 100} m/s, distance $distance, acceleration ${maxAcceleration} cm/s²";
				
				movement.previous.time = galaxy.time;
				
				MovementValues predictionMovVals = { targetPosition, vectorRotate(Vector2l{finalVelocity, 0}, angleToTarget), vectorRotate(Vector2l{maxAcceleration, 0}, angleToTarget) };
				movement.setPredictionBallistic(predictionMovVals, targetPosition, maxAcceleration, galaxy.time + round(timeToTarget));
				thrustComponent.thrustAngle = angleToTarget;
				
				registry.emplace<OnPredictedMovementComponent>(entity);
				
				starSystem.changed<TimedMovementComponent, ThrustComponent>(entity);
				return;
			}
			
			if (movement.next.time == 0) { // && targetEntityID != null
				
				MovementValues targetMovementValue = targetMovement != nullptr ? *targetMovement : MovementValues(targetPosition, targetVelocity, Vector2l {0,0});
				
				const auto result = weaponSystem->getInterceptionPosition2(shipMovementValue, targetMovementValue, 0.0, maxAcceleration / 100.0);
									
				if (!result) {
					
					std::cout << "Unable to find ballistic intercept to target";
					
				} else {
					
					std::cout << "Movement: ballistic prediction to target";
					
					const auto [timeToIntercept, aimPosition, interceptPosition, interceptVelocity, relativeInterceptVelocity] = *result;
					
					std::cout << "timeToIntercept " << timeToIntercept << ", final velocity " << relativeInterceptVelocity.norm() << ", distance1 " << distance << ", distance2 " << (position - interceptPosition).norm() << ", acceleration " << maxAcceleration;
					
					const auto angleToAimTarget = vectorsAngle(position, aimPosition);
					
					movement.previous.time = galaxy.time;
					
					MovementValues predictionMovVals = { interceptPosition, interceptVelocity, vectorRotate(Vector2l(maxAcceleration, 0), angleToAimTarget) };
					movement.setPredictionBallistic(predictionMovVals, aimPosition, maxAcceleration, galaxy.time + timeToIntercept);
					thrustComponent.thrustAngle = angleToAimTarget;
					
					registry.emplace<OnPredictedMovementComponent>(entity);
					
					starSystem.changed<TimedMovementComponent, ThrustComponent>(entity);
					return;
				}
			}
			
			if (timeToTargetWithCurrentSpeed <= 1) {

				position = targetPosition;
				registry.remove_if_exists<MoveToPositionComponent>(entity);
				registry.remove_if_exists<MoveToEntityComponent>(entity);
				std::cout << "Movement: Ballistic target reached time";
				
				starSystem.changed<TimedMovementComponent>(entity);
				return;
			}

			Vector2l tempVelocity = (positionDiff * tickAcceleration) / positionDiffMagnitude;

			if (velocityMagnitute > 10 * currentAcceleration) {
				if (abs(velocityAngleDiffAngleToTarget) <= toRadians(90)) { // Thrust slightly sideways if velocity is only somewhat in the wrong direction
					tempVelocity = vectorRotate(tempVelocity, velocityAngleDiffAngleToTarget);

				} else { // Stop sideways velocity completly if is is too far off target
					tempVelocity = vectorRotate(tempVelocity, std::numbers::pi - velocityAngleDiffAngleToTarget);
				}
			}

			velocity += tempVelocity;
			thrustComponent.thrustAngle = vectorAngle(tempVelocity);

			position += (velocity * delta) / 100;
			
			starSystem.changed<TimedMovementComponent, ThrustComponent>(entity);
			break;
		}
		default: {
			throw std::runtime_error("Unknown approach type: $approach");
		}
	}
}

