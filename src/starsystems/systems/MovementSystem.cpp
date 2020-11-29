/*
 * MovementSystem.cpp
 *
 *  Created on: 22 Nov 2020
 *      Author: exuvo
 */

#include "starsystems/systems/Systems.hpp"
#include "starsystems/components/Components.hpp"
#include "utils/Utils.hpp"

void MovementPreSystem::init() {
//	LOG4CXX_INFO(log, "init");
	
}

void MovementPreSystem::update(delta_type delta) {
//	LOG4CXX_INFO(log, "update");
	
	auto view = registry.view<TimedMovementComponent, ThrustComponent, MassComponent>(entt::exclude_t<OrbitComponent, OnPredictedMovementComponent>{});
	
	for (entt::entity entity : view) {
		TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
		ThrustComponent& thrust = view.get<ThrustComponent>(entity);
		
		if (!registry.any<MoveToPositionComponent, MoveToEntityComponent>(entity)) {
			thrust.thrusting = !movement.get(galaxy.time).value.velocity.isZero();

		} else {
			thrust.thrusting = true;
		}
	}
	
}

void MovementSystem::init() {
//	LOG4CXX_INFO(log, "init");
	
}

void MovementSystem::update(delta_type delta) {
//	LOG4CXX_INFO(log, "update");
	
	auto view = registry.view<TimedMovementComponent>(entt::exclude_t<OrbitComponent, OnPredictedMovementComponent>{});
	
	for (entt::entity entity : view) {
		TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
		
		if (movement.next.time != 0) {
			LOG4CXX_ERROR(log, "Entity " << entity << " on predicted movement but does not have a OnPredictedMovementComponent");
		}
		
		MovementValues shipMovementValue = movement.previous.value;
		
		auto& velocity = shipMovementValue.velocity;
		auto& position = shipMovementValue.position;
		auto& acceleration = shipMovementValue.acceleration;

		if (!registry.any<ThrustComponent, MassComponent>(entity)) {

			if (velocity.isZero()) {
				continue;
			}
			
			position = position + (velocity * delta) / 100;
			
			movement.previous.time = galaxy.time;
			starSystem.changed<TimedMovementComponent>(entity);
			continue;
		}

//		const auto mass = massMapper.get(entityID).mass;
//		const auto massL = mass.toLong();
//		const auto thrustComponent = thrustMapper.get(entityID);
//		
//		const auto currentAcceleration = (100 * thrustComponent.thrust) / massL;
//		const auto maxAcceleration = (100 * thrustComponent.maxThrust) / massL;
//		const auto tickAcceleration = currentAcceleration * deltaGameTime;
//		const auto maxTickAcceleration = maxAcceleration * deltaGameTime;
//
//		if (!DESTINATION_ASPECT.isInterested(entityID)) {
//
//			if (!velocity.isZero()) {
//				const auto velocityMagnitute = velocity.len();
//
//				if (velocityMagnitute < tickAcceleration) {
//
//					velocity = { 0,0 };
//					acceleration = { 0,0 };
//					
//				} else {
//
//					// Apply breaking in same direction as travel
//					tempVelocity.set(velocity).scl(-tickAcceleration).div(velocityMagnitute);
//					
//					if (tickAcceleration == currentAcceleration) {
//						acceleration.set(tempVelocity);
//						
//					} else {
//						acceleration.set(velocity).scl(-currentAcceleration).div(velocityMagnitute);
//					}
//					
//					velocity.add(tempVelocity);
//					
//					thrustComponent.thrustAngle = tempVelocity.angle().toFloat();
//					
//					tempVelocity.set(velocity).scl(deltaGameTime);
//					position.addDiv(tempVelocity, 100);
//					movement.previous.time = galaxy.time;
//				}
//
//				nameMapper.get(entityID).name = "s " + velocity.len().toLong();
//				
//				starSystem.changed(entityID, movementMapper, thrustMapper, nameMapper);
//			}
//
//			continue;
//		}
//
//		const auto moveToPositionComponent = moveToPositionMapper.get(entityID);
//		const auto moveToEntityComponent = moveToEntityMapper.get(entityID);
//
//		const auto targetEntityID = moveToEntityComponent?.targetID;
//		auto targetMovement: MovementValues? = null;
//		const auto targetPosition: Vector2L;
//
//		if (targetEntityID != null) {
//
//			targetMovement = movementMapper.get(targetEntityID).get(galaxy.time).value;
//			targetPosition = targetMovement.position;
//
//		} else {
//
//			targetPosition = moveToPositionComponent.target;
//		}
//
//		const auto approach = if (moveToEntityComponent != null) moveToEntityComponent.approach else moveToPositionComponent.approach;
//
//		tempPosition.set(targetPosition).sub(position);
//		const auto distance = tempPosition.len();
//
//		//TODO if radiuses are touching we are done
//
//		// t = sqrt(2d / a)
//		// d = ut + 1/2at^2
//		// Base formula = t = sqrt(2d / a), Inital speed 0 and reaches target at speed
//		const auto timeToTraverseFrom0Velocity = 2 * FastMath.sqrt((100 * distance) / maxTickAcceleration); 
//
////		println("timeToTraverseFrom0Velocity ${TimeUnits.secondsToString(timeToTraverseFrom0Velocity.toLong())}")
//
//		if (timeToTraverseFrom0Velocity <= 1) {
//			position.set(targetPosition);
//			moveToEntityMapper.remove(entityID);
//			moveToPositionMapper.remove(entityID);
//			println("Movement: target reached distance");
//			starSystem.changed(entityID, movementMapper);
//			continue;
//		}
//
//		const auto targetVelocity = if (targetMovement != null) targetMovement.velocity else Vector2L.Zero;
//		const auto targetVelocityMagnitude = targetVelocity.len();
//		const auto angleToTarget = position.angleTo(targetPosition);
////		const auto velocityAngle = velocity.angle();
//		const auto velocityAngleDiffAngleToTarget = velocity.angle(tempPosition);
////		const auto targetVelocityAngle = targetVelocity.angle()
//		const auto velocityAngleScale = FastMath.cos(targetVelocity.angleToRad(velocity));
//		const auto velocityMagnitute = velocity.len().toLong();
//		const auto timeToTargetWithCurrentSpeed = (100 * distance) / (velocityMagnitute + maxAcceleration);
//		const auto positionDiffMagnitude = tempPosition.len().toLong();
////		println("angleToTarget, $angleToTarget , velocityAngle $velocityAngle")
//
//		switch (approach) {
//			case ApproachType::BRACHISTOCHRONE: {
//
////		const auto relativeSpeed = targetMovement.value.velocity.cpy().sub(shipMovement.value.velocity).len() * FastMath.cos(targetMovement.value.velocity.angleToRad(shipMovement.value.velocity))
//				const auto timeToStop = (velocityMagnitute - velocityAngleScale * targetVelocityMagnitude) / maxAcceleration;
//
////			println("timeToTargetWithCurrentSpeed ${timeToTargetWithCurrentSpeed}, timeToStop ${timeToStop}")
////				println("angleToTarget, $angleToTarget, velocityAngle $velocityAngle, tempVelocityAngle ${tempVelocity.angle()}")
//
////				if (timedMovement != null && (timedMovement.next == null || !timedMovement.next!!.value.position.equals(targetPosition))) {
////
////					timedMovement.setPredictionBallistic()
////				}
//
//				if (timeToTargetWithCurrentSpeed < timeToStop && velocityMagnitute > 0) {
//
//					if (timeToStop <= 1) {
//						position.set(targetPosition);
//						velocity = { 0,0 };
//						acceleration = { 0,0 };
//						
//						moveToEntityMapper.remove(entityID);
//						moveToPositionMapper.remove(entityID);
//						
//						println("Movement: Brachistochrone target reached time");
//						nameMapper.get(entityID).name = "b " + velocity.len().toLong();
//						
//						starSystem.changed(entityID, movementMapper, nameMapper);
//						continue;
//					}
//
//					// Apply breaking in same direction as travel
//					tempVelocity.set(velocity).scl(-tickAcceleration).div(velocityMagnitute);
//					
//					if (tickAcceleration == currentAcceleration) {
//						acceleration.set(tempVelocity);
//						
//					} else {
//						acceleration.set(velocity).scl(-currentAcceleration).div(velocityMagnitute);
//					}
//					
//					velocity.add(tempVelocity);
//					
//					const auto thrustReverseAngle = (tempVelocity.angle() + 180) % 360;
//					thrustComponent.thrustAngle = thrustReverseAngle.toFloat();
//					
//					nameMapper.get(entityID).name = "b " + velocity.len().toLong();
//
//				} else {
//
//					tempVelocity.set(tempPosition).scl(tickAcceleration).div(positionDiffMagnitude);
//					
//					if (tickAcceleration == currentAcceleration) {
//						acceleration.set(tempVelocity);
//						
//					} else {
//						acceleration.set(tempPosition).scl(currentAcceleration).div(positionDiffMagnitude);
//					}
//
//					if (velocityMagnitute > 10 * currentAcceleration) {
//						if (FastMath.abs(velocityAngleDiffAngleToTarget) <= 90) { // Thrust slightly sideways if velocity is only somewhat in the wrong direction
//							tempVelocity.rotate(velocityAngleDiffAngleToTarget);
////							nameMapper.get(entityID).name = "< " + velocityAngleDiffAngleToTarget
//
//						} else { // Stop sideways velocity completly if is is too far off target
//							tempVelocity.rotate(180 - velocityAngleDiffAngleToTarget);
////							nameMapper.get(entityID).name = "> " + velocityAngleDiffAngleToTarget + "," + (180 - velocityAngleDiffAngleToTarget)
//						}
//					}
//
//					velocity.add(tempVelocity);
//					
//					thrustComponent.thrustAngle = tempVelocity.angle().toFloat();
//					
//					nameMapper.get(entityID).name = "a " + velocity.len().toLong();
//					
////					println("angleToTarget $angleToTarget, velocityAngle $velocityAngle, angleToTargetDiffVelocityAngle $angleToTargetDiffVelocityAngle, tickAcceleration $tickAcceleration cm/s, acceleration $acceleration cm/s, tempVelocity $tempVelocity, velocity $velocity, distance $distance")
//				}
//
//				tempVelocity.set(velocity).scl(deltaGameTime);
//				position.addDiv(tempVelocity, 100);
//
////				println("tickAcceleration $tickAcceleration cm/s, acceleration $acceleration cm/s, velocity $velocity, distance $distance")
//				starSystem.changed(entityID, movementMapper, thrustMapper, nameMapper);
//				break;
//			}
//			case ApproachType::BALLISTIC: {
//
//				nameMapper.get(entityID).name = "a " + velocityMagnitute;
//
//				if (movement.next == null && targetEntityID == null && velocityMagnitute == 0L) {
//
//					println("Movement: ballistic prediction to target");
//
//					const auto timeToTarget = FastMath.sqrt((2 * 100 * distance) / maxAcceleration.toDouble());
//
//					const auto finalVelocity = maxAcceleration * timeToTarget;
//					
//					println("timeToTarget $timeToTarget, final velocity ${finalVelocity / 100} m/s, distance $distance, acceleration ${maxAcceleration} cm/s²");
//					
//					tempVelocity.set(finalVelocity.toLong(), 0).rotate(angleToTarget);
//					
//					movement.previous.time = galaxy.time;
//					const auto targetPositionCpy = targetPosition.cpy();
//					movement.setPredictionBallistic(MovementValues(targetPositionCpy, tempVelocity.cpy(), Vector2L(maxAcceleration, 0).rotate(angleToTarget)), targetPositionCpy, maxAcceleration, galaxy.time + FastMath.round(timeToTarget));
//					thrustComponent.thrustAngle = angleToTarget.toFloat();
//					
//					predictedMovementMapper.create(entityID);
//					
//					starSystem.changed(entityID, movementMapper, thrustMapper);
//					continue;
//				}
//				
//				if (movement.next == null) { // && targetEntityID != null
//					
//					const auto targetMovementValue: MovementValues;
//					
//					if (targetMovement != null) {
//						targetMovementValue = targetMovement;
//						
//					} else {
//						targetMovementValue = MovementValues(targetPosition, targetVelocity, Vector2L.Zero);
//					}
//					
//					const auto result = weaponSystem.getInterceptionPosition2(shipMovementValue, targetMovementValue, 0.0, maxAcceleration.toDouble() / 100);
//										
//					if (result == null) {
//						
//						println("Unable to find ballistic intercept to target");
//						
//					} else {
//						
//						println("Movement: ballistic prediction to target");
//						
//						const auto (timeToIntercept, aimPosition, interceptPosition, interceptVelocity, relativeInterceptVelocity) = result;
//						
//						println("timeToIntercept $timeToIntercept, final velocity ${relativeInterceptVelocity.len()}, distance1 $distance, distance2 ${position.cpy().sub(interceptPosition).len()}, acceleration $maxAcceleration");
//						
//						const auto angleToAimTarget = position.angleTo(aimPosition);
//						const auto angleRadToAimTarget = position.angleRad(aimPosition);
//						
//						movement.previous.time = galaxy.time;
//						movement.setPredictionBallistic(MovementValues(interceptPosition, interceptVelocity, Vector2L(maxAcceleration, 0).rotate(angleRadToAimTarget)), aimPosition, maxAcceleration, galaxy.time + timeToIntercept);
//						thrustComponent.thrustAngle = angleToAimTarget.toFloat();
//						
//						predictedMovementMapper.create(entityID);
//						
//						starSystem.changed(entityID, movementMapper, thrustMapper);
//						continue
//					}
//				}
//				
//				if (timeToTargetWithCurrentSpeed <= 1) {
//
//					position.set(targetPosition);
//					moveToEntityMapper.remove(entityID);
//					moveToPositionMapper.remove(entityID);
//					println("Movement: Ballistic target reached time");
//					
//					starSystem.changed(entityID, movementMapper);
//					continue;
//				}
//
//				tempVelocity.set(tempPosition).scl(tickAcceleration).div(positionDiffMagnitude);
//
//				if (velocityMagnitute > 10 * currentAcceleration) {
//					if (FastMath.abs(velocityAngleDiffAngleToTarget) <= 90) { // Thrust slightly sideways if velocity is only somewhat in the wrong direction
//						tempVelocity.rotate(velocityAngleDiffAngleToTarget);
//
//					} else { // Stop sideways velocity completly if is is too far off target
//						tempVelocity.rotate(180 - velocityAngleDiffAngleToTarget);
//					}
//				}
//
//				velocity.add(tempVelocity);
//				thrustComponent.thrustAngle = tempVelocity.angle().toFloat();
//
//				tempVelocity.set(velocity).scl(deltaGameTime);
//				position.addDiv(tempVelocity, 100);
//				
//				starSystem.changed(entityID, movementMapper, thrustMapper);
//				break;
//			}
//			default: {
//				throw RuntimeException("Unknown approach type: $approach");
//			}
//		}
		
	}
}

