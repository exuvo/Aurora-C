/*
 * WeaponSystem.cpp
 *
 *  Created on: 1 Dec 2020
 *      Author: exuvo
 */

#include <iostream>
//#include <boost/container/static_vector.hpp>

#include "starsystems/systems/Systems.hpp"
#include "utils/Utils.hpp"

void WeaponSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
}

void WeaponSystem::update(delta_type delta) {
	
}

	/**
	 * Calculates intercept with position, velocity, timed varying acceleration and then coasting
	 * Assumes timeToIntercept will be longer than thrust time
	 */
std::optional<InterceptResult> WeaponSystem::getInterceptionPosition5(MovementValues shooterMovement, // in m, cm/s, cm/s²
                                                                      MovementValues targetMovement, // in m, cm/s, cm/s²
                                                                      double missileLaunchSpeed, // in m/s
                                                                      double missileStartAcceleration, // in m/s²
                                                                      double missileEndAcceleration, // in m/s²
                                                                      double missileAccelTime // in s
) {
		//TODO implement
//		const auto targetAcceleration = Vector2d(targetMovement.acceleration.x.toDouble(), targetMovement.acceleration.y.toDouble()).div(100.0)
//		const auto relativeVelocity = Vector2d((targetMovement.velocity.x - shooterMovement.velocity.x).toDouble(), (targetMovement.velocity.y - shooterMovement.velocity.y).toDouble()).div(100.0)
//		const auto relativePosition = Vector2d((targetMovement.position.x - shooterMovement.position.x).toDouble(), (targetMovement.position.y - shooterMovement.position.y).toDouble())
//
//		// The math behind it is this:
//		// (A+B+C)^2 = (A+B+C).(A+B+C) = A.A + B.B + C.C + 2*(A.B + A.C + B.C)
//
//		// interception possible, when
//		// | (P + V*t + A/2 * t^2) | - (v*t + 1/2 * a * t^2) = 0
//
//		// (target/left side:)
//		// -> P'(t) = P.P + V.V * t^2 + 1/4 * A.A * t^4 + 2 * P.V * t + 2/2 * P.A * t^2 + 2/2 * V.A * t^3
//		// = P.P + 2 * P.V * t + (V.V + P.A) * t^2 + V.A * t^3 + 1/4 * A.A * t^4
//
//		// (interceptor/right)
//		// -> v^2 * t^2 + 1/4 * a^2 * t^4 + v*a * t^3
//
//		// final polynomial:
//		// (1/4 * A.A - 1/4 * a^2) * t^4
//		// (V.A - v*a) * t^3
//		// (V.V + P.A - v^2) * t^2
//		// (2 * P.V) * t^1
//		// (P.P) * t^0
//
//		const auto coefs = DoubleArray(5)
//		coefs[4] = targetAcceleration.dot(targetAcceleration) / 4.0 - (missileAcceleration * missileAcceleration) / 4.0
//		coefs[3] = relativeVelocity.dot(targetAcceleration) - missileLaunchSpeed * missileAcceleration
//		coefs[2] = relativePosition.dot(targetAcceleration) + relativeVelocity.dot(relativeVelocity) - (missileLaunchSpeed * missileLaunchSpeed)
//		coefs[1] = 2 * relativePosition.dot(relativeVelocity)
//		coefs[0] = relativePosition.dot(relativePosition)
//
//		try {
//			auto initialGuess = getPositiveRootOfQuadraticEquation(missileEndAcceleration - missileStartAcceleration, missileLaunchSpeed, -relativePosition.len())
//
//			if (initialGuess.isNaN() || initialGuess <= 0) {
//				log.warn("invalid initialGuess $initialGuess")
//				initialGuess = 1.0
//			}
//
//			const auto complexRoots = polynomialSolver.solveAllComplex(coefs, initialGuess, POLYNOMIAL_MAX_ITERATIONS)
//
//			auto solvedTime: Double? = null
//
//			if (complexRoots != null) {
//				for (root in complexRoots) {
////					println("complex root $root")
//					if (root.getImaginary() == 0.0 && root.getReal() > 0 && (solvedTime == null || root.getReal() < solvedTime)) {
//						solvedTime = root.getReal()
//					}
//				}
//			}
//
////			println("solvedTime $solvedTime")
//
//			if (solvedTime != null) {
//
//				// RPos + t * RVel + (t^2 * TAccel) / 2
//				const auto relativeAimPosition = relativePosition.set(relativeVelocity).scl(solvedTime).add(targetAcceleration.scl(0.5 * solvedTime * solvedTime))
//
//				const auto aimPosition = targetMovement.position.cpy()
//				aimPosition.add(relativeAimPosition.x.toLong(), relativeAimPosition.y.toLong())
//
//				const auto addedVelocity = relativePosition.set(missileLaunchSpeed + missileAcceleration * solvedTime, 0.0).rotateRad(shooterMovement.position.angleToRad(aimPosition)).scl(100.0)
//				relativeVelocity.scl(100.0).add(addedVelocity)
//				const auto interceptVelocity = shooterMovement.velocity.cpy().add(addedVelocity.x.toLong(), addedVelocity.y.toLong())
//
//				const auto interceptPosition = targetMovement.acceleration.cpy().scl(0.5 * solvedTime * solvedTime)
//				interceptPosition.mulAdd(targetMovement.velocity, FastMath.round(solvedTime))
//				interceptPosition.div(100).add(targetMovement.position)
//
//				return InterceptResult(FastMath.round(solvedTime), aimPosition, interceptPosition, interceptVelocity, Vector2l(relativeVelocity.x.toLong(), relativeVelocity.y.toLong()))
//			}
//
//		} catch (e: TooManyEvaluationsException) {
//			log.warn("Unable to solve intercept polynomial: " + e.message)
//		}

	return {};
}
	
/**
 * Calculates intercept with position, velocity, timed constant acceleration and then coasting
 * Assumes timeToIntercept will be longer than thrust time
 */
std::optional<InterceptResult> WeaponSystem::getInterceptionPosition4(MovementValues shooterMovement, // in m, cm/s, cm/s²
                                                                      MovementValues targetMovement, // in m, cm/s, cm/s²
                                                                      double missileLaunchSpeed, // in m/s
                                                                      double missileAcceleration, // in m/s²
                                                                      double missileAccelTime // in s
) {
		//TODO implement
//		const auto targetAcceleration = Vector2d(targetMovement.acceleration.x.toDouble(), targetMovement.acceleration.y.toDouble()).div(100.0)
//		const auto relativeVelocity = Vector2d((targetMovement.velocity.x - shooterMovement.velocity.x).toDouble(), (targetMovement.velocity.y - shooterMovement.velocity.y).toDouble()).div(100.0)
//		const auto relativePosition = Vector2d((targetMovement.position.x - shooterMovement.position.x).toDouble(), (targetMovement.position.y - shooterMovement.position.y).toDouble())
//
//		// The math behind it is this:
//		// (A+B+C)^2 = (A+B+C).(A+B+C) = A.A + B.B + C.C + 2*(A.B + A.C + B.C)
//
//		// interception possible, when
//		// | (P + V*t + A/2 * t^2) | - (v*t + 1/2 * a * t^2) = 0
//
//		// (target/left side:)
//		// -> P'(t) = P.P + V.V * t^2 + 1/4 * A.A * t^4 + 2 * P.V * t + 2/2 * P.A * t^2 + 2/2 * V.A * t^3
//		// = P.P + 2 * P.V * t + (V.V + P.A) * t^2 + V.A * t^3 + 1/4 * A.A * t^4
//
//		// (interceptor/right)
//		// -> v^2 * t^2 + 1/4 * a^2 * t^4 + v*a * t^3
//
//		// final polynomial:
//		// (1/4 * A.A - 1/4 * a^2) * t^4
//		// (V.A - v*a) * t^3
//		// (V.V + P.A - v^2) * t^2
//		// (2 * P.V) * t^1
//		// (P.P) * t^0
//
//		const auto coefs = DoubleArray(5)
//		coefs[4] = targetAcceleration.dot(targetAcceleration) / 4.0 - (missileAcceleration * missileAcceleration) / 4.0
//		coefs[3] = relativeVelocity.dot(targetAcceleration) - missileLaunchSpeed * missileAcceleration
//		coefs[2] = relativePosition.dot(targetAcceleration) + relativeVelocity.dot(relativeVelocity) - (missileLaunchSpeed * missileLaunchSpeed)
//		coefs[1] = 2 * relativePosition.dot(relativeVelocity)
//		coefs[0] = relativePosition.dot(relativePosition)
//
//		try {
//			auto initialGuess = getPositiveRootOfQuadraticEquation(missileAcceleration, missileLaunchSpeed, -relativePosition.len())
//
//			if (initialGuess.isNaN() || initialGuess <= 0) {
//				log.warn("invalid initialGuess $initialGuess")
//				initialGuess = 1.0
//			}
//
//			const auto complexRoots = polynomialSolver.solveAllComplex(coefs, initialGuess, POLYNOMIAL_MAX_ITERATIONS)
//
//			auto solvedTime: Double? = null
//
//			if (complexRoots != null) {
//				for (root in complexRoots) {
////					println("complex root $root")
//					if (root.getImaginary() == 0.0 && root.getReal() > 0 && (solvedTime == null || root.getReal() < solvedTime)) {
//						solvedTime = root.getReal()
//					}
//				}
//			}
//
////			println("solvedTime $solvedTime")
//
//			if (solvedTime != null) {
//
//				// RPos + t * RVel + (t^2 * TAccel) / 2
//				const auto relativeAimPosition = relativePosition.set(relativeVelocity).scl(solvedTime).add(targetAcceleration.scl(0.5 * solvedTime * solvedTime))
//
//				const auto aimPosition = targetMovement.position.cpy()
//				aimPosition.add(relativeAimPosition.x.toLong(), relativeAimPosition.y.toLong())
//
//				const auto addedVelocity = relativePosition.set(missileLaunchSpeed + missileAcceleration * solvedTime, 0.0).rotateRad(shooterMovement.position.angleToRad(aimPosition)).scl(100.0)
//				relativeVelocity.scl(100.0).add(addedVelocity)
//				const auto interceptVelocity = shooterMovement.velocity.cpy().add(addedVelocity.x.toLong(), addedVelocity.y.toLong())
//
//				const auto interceptPosition = targetMovement.acceleration.cpy().scl(0.5 * solvedTime * solvedTime)
//				interceptPosition.mulAdd(targetMovement.velocity, FastMath.round(solvedTime))
//				interceptPosition.div(100).add(targetMovement.position)
//
//				return InterceptResult(FastMath.round(solvedTime), aimPosition, interceptPosition, interceptVelocity, Vector2l(relativeVelocity.x.toLong(), relativeVelocity.y.toLong()))
//			}
//
//		} catch (e: TooManyEvaluationsException) {
//			log.warn("Unable to solve intercept polynomial: " + e.message)
//		}
	
	return {};
}
	
/**
 * Calculates intercept with position, velocity and varying acceleration
 */
std::optional<InterceptResult> WeaponSystem::getInterceptionPosition3(MovementValues shooterMovement, // in m, cm/s, cm/s²
                                                                      MovementValues targetMovement, // in m, cm/s, cm/s²
                                                                      double missileLaunchSpeed, // in m/s
                                                                      double missileStartAcceleration, // in m/s²
                                                                      double missileEndAcceleration // in m/s²
) {
		//TODO implement
//		const auto targetAcceleration = Vector2d(targetMovement.acceleration.x.toDouble(), targetMovement.acceleration.y.toDouble()).div(100.0)
//		const auto relativeVelocity = Vector2d((targetMovement.velocity.x - shooterMovement.velocity.x).toDouble(), (targetMovement.velocity.y - shooterMovement.velocity.y).toDouble()).div(100.0)
//		const auto relativePosition = Vector2d((targetMovement.position.x - shooterMovement.position.x).toDouble(), (targetMovement.position.y - shooterMovement.position.y).toDouble())
//
//		// The math behind it is this:
//		// (A+B+C)^2 = (A+B+C).(A+B+C) = A.A + B.B + C.C + 2*(A.B + A.C + B.C)
//
//		// interception possible, when
//		// | (P + V*t + A/2 * t^2) | - (v*t + 1/2 * a * t^2) = 0
//
//		// (target/left side:)
//		// -> P'(t) = P.P + V.V * t^2 + 1/4 * A.A * t^4 + 2 * P.V * t + 2/2 * P.A * t^2 + 2/2 * V.A * t^3
//		// = P.P + 2 * P.V * t + (V.V + P.A) * t^2 + V.A * t^3 + 1/4 * A.A * t^4
//
//		// (interceptor/right)
//		// -> v^2 * t^2 + 1/4 * a^2 * t^4 + v*a * t^3
//
//		// final polynomial:
//		// (1/4 * A.A - 1/4 * a^2) * t^4
//		// (V.A - v*a) * t^3
//		// (V.V + P.A - v^2) * t^2
//		// (2 * P.V) * t^1
//		// (P.P) * t^0
//
//		const auto coefs = DoubleArray(5)
//		coefs[4] = targetAcceleration.dot(targetAcceleration) / 4.0 - (missileAcceleration * missileAcceleration) / 4.0
//		coefs[3] = relativeVelocity.dot(targetAcceleration) - missileLaunchSpeed * missileAcceleration
//		coefs[2] = relativePosition.dot(targetAcceleration) + relativeVelocity.dot(relativeVelocity) - (missileLaunchSpeed * missileLaunchSpeed)
//		coefs[1] = 2 * relativePosition.dot(relativeVelocity)
//		coefs[0] = relativePosition.dot(relativePosition)
//
//		try {
//			auto initialGuess = getPositiveRootOfQuadraticEquation(missileEndAcceleration - missileStartAcceleration, missileLaunchSpeed, -relativePosition.len())
//
//			if (initialGuess.isNaN() || initialGuess <= 0) {
//				log.warn("invalid initialGuess $initialGuess")
//				initialGuess = 1.0
//			}
//
//			const auto complexRoots = polynomialSolver.solveAllComplex(coefs, initialGuess, POLYNOMIAL_MAX_ITERATIONS)
//
//			auto solvedTime: Double? = null
//
//			if (complexRoots != null) {
//				for (root in complexRoots) {
////					println("complex root $root")
//					if (root.getImaginary() == 0.0 && root.getReal() > 0 && (solvedTime == null || root.getReal() < solvedTime)) {
//						solvedTime = root.getReal()
//					}
//				}
//			}
//
////			println("solvedTime $solvedTime")
//
//			if (solvedTime != null) {
//
//				// RPos + t * RVel + (t^2 * TAccel) / 2
//				const auto relativeAimPosition = relativePosition.set(relativeVelocity).scl(solvedTime).add(targetAcceleration.scl(0.5 * solvedTime * solvedTime))
//
//				const auto aimPosition = targetMovement.position.cpy()
//				aimPosition.add(relativeAimPosition.x.toLong(), relativeAimPosition.y.toLong())
//
//				const auto addedVelocity = relativePosition.set(missileLaunchSpeed + missileAcceleration * solvedTime, 0.0).rotateRad(shooterMovement.position.angleToRad(aimPosition)).scl(100.0)
//				relativeVelocity.scl(100.0).add(addedVelocity)
//				const auto interceptVelocity = shooterMovement.velocity.cpy().add(addedVelocity.x.toLong(), addedVelocity.y.toLong())
//
//				const auto interceptPosition = targetMovement.acceleration.cpy().scl(0.5 * solvedTime * solvedTime)
//				interceptPosition.mulAdd(targetMovement.velocity, FastMath.round(solvedTime))
//				interceptPosition.div(100).add(targetMovement.position)
//
//				return InterceptResult(FastMath.round(solvedTime), aimPosition, interceptPosition, interceptVelocity, Vector2l(relativeVelocity.x.toLong(), relativeVelocity.y.toLong()))
//			}
//
//		} catch (e: TooManyEvaluationsException) {
//			log.warn("Unable to solve intercept polynomial: " + e.message)
//		}

	return {};
}
	
	/**
	 * Calculates intercept with position, velocity and constant acceleration
 	 */
	std::optional<InterceptResult> WeaponSystem::getInterceptionPosition2(MovementValues shooterMovement, // in m, cm/s, cm/s²
                                                                      MovementValues targetMovement, // in m, cm/s, cm/s²
                                                                      double missileLaunchSpeed, // in m/s
                                                                      double missileAcceleration // in m/s²
	) {
		/**
		https://www.gamedev.net/forums/topic/579481-advanced-intercept-equation
		https://www.gamedev.net/forums/?topic_id=401165&page=2
		https://www.gamedev.net/forums/topic/621460-need-help-with-interception-of-accelerated-target/
		**/
		
		Vector2d targetAcceleration = targetMovement.acceleration.cast<double>() / 100.0;
		Vector2d relativeVelocity = (targetMovement.velocity - shooterMovement.velocity).cast<double>() / 100.0;
		Vector2d relativePosition = (targetMovement.position - shooterMovement.position).cast<double>();
		
		// The math behind it is this:
		// (A+B+C)^2 = (A+B+C).(A+B+C) = A.A + B.B + C.C + 2*(A.B + A.C + B.C)
		
		// interception possible, when
		// | (P + V*t + A/2 * t^2) | - (v*t + 1/2 * a * t^2) = 0
		
		// (target/left side:)
		// -> P'(t) = P.P + V.V * t^2 + 1/4 * A.A * t^4 + 2 * P.V * t + 2/2 * P.A * t^2 + 2/2 * V.A * t^3
		// = P.P + 2 * P.V * t + (V.V + P.A) * t^2 + V.A * t^3 + 1/4 * A.A * t^4
		
		// (interceptor/right)
		// -> v^2 * t^2 + 1/4 * a^2 * t^4 + v*a * t^3
		
		// final polynomial:
		// (1/4 * A.A - 1/4 * a^2) * t^4
		// (V.A - v*a) * t^3
		// (V.V + P.A - v^2) * t^2
		// (2 * P.V) * t^1
		// (P.P) * t^0
		
//		boost::container::static_vector<double, 5> coefs;
//		std::vector<double> coefs;
		Eigen::Matrix<double, 5, 1> coefs;
		coefs[4] = targetAcceleration.dot(targetAcceleration) / 4.0 - (missileAcceleration * missileAcceleration) / 4.0;
		coefs[3] = relativeVelocity.dot(targetAcceleration) - missileLaunchSpeed * missileAcceleration;
		coefs[2] = relativePosition.dot(targetAcceleration) + relativeVelocity.dot(relativeVelocity) - (missileLaunchSpeed * missileLaunchSpeed);
		coefs[1] = 2 * relativePosition.dot(relativeVelocity);
		coefs[0] = relativePosition.dot(relativePosition);
		
//		try {
			double initialGuess = getPositiveRootOfQuadraticEquation(missileAcceleration, missileLaunchSpeed, -relativePosition.norm());
			
			if (std::isnan(initialGuess) || initialGuess <= 0) {
				LOG4CXX_WARN(log, "invalid initialGuess $initialGuess");
				initialGuess = 1.0;
			}
			
//			polynomialSolver.compute(coefs);
//			const Eigen::Matrix<std::complex<double>, 4, 1>& complexRoots = polynomialSolver.roots();
			
//			const auto complexRoots = polynomialSolver.solveAllComplex(coefs, initialGuess, POLYNOMIAL_MAX_ITERATIONS);
			
			std::optional<double> solvedTime = {};
			
//			if (complexRoots != null) {
//				for (const std::complex<double>& root : complexRoots) {
////					println("complex root $root")
//					if (root.imag() == 0.0 && root.real() > 0 && (!solvedTime || root.real() < *solvedTime)) {
//						solvedTime = root.real();
//					}
//				}
//			}
			
			std::cout << "solvedTime $solvedTime, initialGuess $initialGuess";
			
			if (solvedTime) {
				
				// RPos + t * RVel + (t^2 * TAccel) / 2
				Vector2l relativeAimPosition = (relativeVelocity * *solvedTime + targetAcceleration * 0.5 * *solvedTime * *solvedTime).cast<int64_t>();
				
				Vector2l aimPosition = targetMovement.position + relativeAimPosition;
				
				Vector2d addedVelocity = (vectorRotate(Vector2d{missileLaunchSpeed + missileAcceleration * *solvedTime, 0.0}, vectorsAngle(shooterMovement.position, aimPosition)) * 100.0);
				relativeVelocity = relativeVelocity * 100 + addedVelocity;
				Vector2l interceptVelocity = shooterMovement.velocity + addedVelocity.cast<int64_t>();
				
				Vector2l interceptPosition = targetMovement.acceleration * (int64_t) round(0.5 * *solvedTime * *solvedTime) + targetMovement.velocity * (int64_t) round(*solvedTime);
				interceptPosition = interceptPosition / 100 + targetMovement.position;
				
				return InterceptResult { (uint64_t) round(*solvedTime), aimPosition, interceptPosition, interceptVelocity, relativeVelocity.cast<int64_t>() };
			}
			
//		} catch (TooManyEvaluationsException e) {
//			log.warn("Unable to solve intercept polynomial: " + e.message)
//		}
		
		return {};
	}
	
/**
 * Calculates intercept with position and velocity https://www.gamedev.net/forums/?topic_id=401165
 */
std::optional<InterceptResult> WeaponSystem::getInterceptionPosition1(MovementValues shooterMovement,
                                                                      MovementValues targetMovement,
                                                                      double projectileSpeed
) {
	Vector2d relativeVelocity = Vector2d(targetMovement.velocity.x() - shooterMovement.velocity.x(), targetMovement.velocity.y() - shooterMovement.velocity.y()) / 100.0;
	Vector2l relativePosition = targetMovement.position - shooterMovement.position;
	
	const double a = projectileSpeed * projectileSpeed - relativeVelocity.dot(relativeVelocity);
	const double b = 2 * relativeVelocity.dot(relativePosition.cast<double>());
	const double c = relativePosition.dot(-relativePosition);
	
	const std::optional<double> root = getPositiveRootOfQuadraticEquationSafe(a, b, c);
	
	if (!root || *root < 0) { // Intercept is not possible
		return {};
	}
	
//		println("simple root $root")
	
	relativeVelocity *= *root;
	
	Vector2l aimPosition = targetMovement.position + relativeVelocity.cast<int64_t>();
	
	Vector2l interceptPosition = (targetMovement.velocity * (uint64_t) round(*root)) / 100 + targetMovement.position;
	
	Vector2d projectileVelocity = vectorRotate(Vector2d{projectileSpeed, 0.0}, vectorsAngle(shooterMovement.position, aimPosition));
	relativeVelocity = (relativeVelocity + projectileVelocity) * 100.0;
	
	return InterceptResult { (uint64_t) round(*root), aimPosition, interceptPosition, projectileVelocity.cast<int64_t>(), relativeVelocity.cast<int64_t>()};
}
