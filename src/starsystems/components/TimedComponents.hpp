/*
 * TimedComponents.hpp
 *
 *  Created on: 23 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_TIMEDCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_TIMEDCOMPONENTS_HPP_

#include "entt/entt.hpp"

#include "MovementComponents.hpp"
#include "utils/Math.hpp"

// In m, cm/s, cm/s�
struct MovementValues {
		Vector2l position;
		Vector2l velocity;
		Vector2l acceleration;
		
		MovementValues() {
			position = {0, 0};
			velocity = {0, 0};
			acceleration = {0, 0};
		}
		
		MovementValues(Vector2l position2, Vector2l velocity2, Vector2l acceleration2) {
			position = position2;
			velocity = velocity2;
			acceleration = acceleration2;
		}
		
		MovementValues(uint32_t x, uint32_t y, uint32_t vx, uint32_t vy, uint32_t ax, uint32_t ay) {
			position = {x, y};
			velocity = {vx, vy};
			acceleration = {ax, ay};
		}
		
		Vector2l getPositionInKM() {
			return Vector2l { (signum(position.x()) * 500 + position.x()) / 1000, (signum(position.y()) * 500 + position.y()) / 1000};
		}
};

template<typename T>
struct TimedValue {
		T value;
		uint64_t time;
};

template<typename T>
struct TimedComponent {
		virtual TimedValue<T> get(uint64_t time) = 0;
		
		virtual ~TimedComponent() = default;
};

//TODO support multiple future points
//TODO save old points
template<typename T>
struct InterpolatedComponent : public TimedComponent<T> {
		using Tval = TimedValue<T>;
		
		Tval previous;
		Tval interpolated;
		Tval next;
		
		InterpolatedComponent(Tval initial) {
			previous = initial;
			interpolated.time = 0; // 0 = invalid as only previous could ever have 0
			next.time = 0;
		}
		
		virtual void setValue(Tval& timedValue, const T& newValue) = 0;
		
		void set(T& value, uint64_t time) {
			if (previous.time > time) {
				return;
			}
			
			if (next.time != 0 && time >= next.time) {
				next.time = 0;
			}
			
			setValue(previous, value);
			previous.time = time;
		}
		
		virtual bool setPrediction(const  T& value, uint64_t time) {
	
			if (previous.time >= time) {
				next.time = 0; // why?
				return false;
			}
	
			setValue(next, value);
			next.time = time;
	
			return true;
		}
	
		virtual void interpolate(uint64_t time) = 0;

		virtual Tval get(uint64_t time) override {
			
			if (time <= previous.time || next.time == 0) {
				return previous;
			}
			
			if (next.time != 0 && time >= next.time) {
				return next;
			}
			
			if (interpolated.time != time) {
				interpolate(time);
				interpolated.time = time;
			}
			
			return interpolated;
		}
};

struct TimedMovementComponent: InterpolatedComponent<MovementValues> {
	using Tval = TimedValue<MovementValues>;
	ApproachType approach = ApproachType::COAST;
	int64_t startAcceleration = 0; // = null
	int64_t finalAcceleration = 0; // = null
	Vector2l aimTarget = { 0, 0 }; // = null
	
	TimedMovementComponent() : InterpolatedComponent<MovementValues>(Tval { MovementValues {}, 0 }) {}
	
	virtual void setValue(Tval& timedValue, const MovementValues& newValue) override {
		timedValue.value = newValue;
	}
	
	TimedMovementComponent& set(int64_t x, int64_t y, int64_t vx, int64_t vy, int64_t ax, int64_t ay, uint64_t time)  {

		if (previous.time > time) {
			return *this;
		}

		if (next.time != 0 && time >= next.time) {
			next.time = 0;
		}

		previous.value.position = {x, y};
		previous.value.velocity = {vx, vy};
		previous.value.acceleration = {ax, ay};
		previous.time = time;
		return *this;
	}
	
	TimedMovementComponent& set(const Vector2l& position, const Vector2l& velocity, const Vector2l& acceleration, uint64_t time) {

		if (previous.time > time) {
			return *this;
		}

		if (next.time != 0 && time >= next.time) {
			next.time = 0;
		}

		previous.value.position = position;
		previous.value.velocity = velocity;
		previous.value.acceleration = acceleration;
		previous.time = time;
		
		return *this;
	}
	
//	virtual void reset() override {
//		previous.value.position = {0, 0};
//		previous.value.velocity = {0, 0};
//		previous.value.acceleration = {0, 0};
//		previous.time = 0;
//		
//		interpolated.time = 0;
//		next.time = 0;
//	}
	
	virtual bool setPrediction(const MovementValues& value, uint64_t time) override {

		if (InterpolatedComponent::setPrediction(value, time)) {
			approach = ApproachType::COAST;
			startAcceleration = 0; // = null
			finalAcceleration = 0; // = null
			aimTarget = { 0, 0 }; // = null
			
			if (interpolated.time != 0) {
				interpolated.time = 0;
			}
			
			return true;
		}

		return false;
	}
	
	bool setPredictionCoast(const MovementValues& value, const Vector2l& aimTarget, uint64_t time) {

		if (InterpolatedComponent::setPrediction(value, time)) {
			approach = ApproachType::COAST;
			startAcceleration = 0; // = null
			finalAcceleration = 0; // = null
			this->aimTarget = aimTarget;
			
			if (interpolated.time != 0) {
				interpolated.time = 0;
			}
			
			return true;
		}

		return false;
	}

	bool setPredictionBrachistocrone(const MovementValues& value, int64_t startAcceleration, uint64_t time) {

		if (InterpolatedComponent::setPrediction(value, time)) {
			approach = ApproachType::BRACHISTOCHRONE;
			this->startAcceleration = startAcceleration;
			this->finalAcceleration = (int64_t) value.acceleration.norm(); // len
			aimTarget = { 0, 0 }; // = null
			
			if (interpolated.time != 0) {
				interpolated.time = 0;
			}
			
			return true;
		}

		return false;
	}

	bool setPredictionBallistic(const MovementValues& value, const Vector2l& aimTarget, int64_t startAcceleration, uint64_t time) {

		if (InterpolatedComponent::setPrediction(value, time)) {
			approach = ApproachType::BALLISTIC;
			this->startAcceleration = startAcceleration;
			this->finalAcceleration = (int64_t) value.acceleration.norm(); // len
			this->aimTarget = aimTarget;
			
			if (interpolated.time != 0) {
				interpolated.time = 0;
			}
			
			auto averageAcceleration = (startAcceleration + finalAcceleration) / 2;
			auto startPosition = previous.value.position;
			auto angle = vectorsAngle(startPosition, aimTarget);
			
			interpolated.value.acceleration = vectorRotate(Vector2l{averageAcceleration, 0}, angle);
			
			return true;
		}

		return false;
	}

	virtual void interpolate(uint64_t time) override {

		Vector2l& startPosition = previous.value.position;
		Vector2l& endPosition = next.value.position;
		uint64_t startTime = previous.time;
		uint64_t endTime = next.time;
		Vector2l& startVelocity = previous.value.velocity;
//		Vector2l& finalVelocity = next.value.velocity;

		Vector2l& position = interpolated.value.position;
		Vector2l& velocity = interpolated.value.velocity;
//		val acceleration = interpolated.value.acceleration;

//		position.set(startPosition).sub(endPosition);
//		val totalDistance = position.len();
		uint64_t travelTime = endTime - startTime;
		uint64_t traveledTime = time - startTime;

		if (traveledTime > travelTime) {
			throw std::runtime_error("Invalid state: startTime $startTime, endTime $endTime, traveledTime $traveledTime");
		}

		switch (approach) {
			case ApproachType::COAST: {
				position = vectorsLerp(startPosition, endPosition, traveledTime, travelTime);
				break;
			}
			case ApproachType::BALLISTIC: {
				int64_t averageAcceleration = (startAcceleration + finalAcceleration) / 2;
				
				int64_t distanceTraveled = (averageAcceleration * traveledTime * traveledTime) / (2 * 100);
				
				int64_t aimDistance = Eigen::numext::round((startPosition - aimTarget).norm());
				position = vectorsLerp(startPosition, aimTarget, distanceTraveled, aimDistance);
				
				velocity = (startVelocity * traveledTime) / 100;
				position += velocity;
				
				double angle = vectorsAngle(startPosition, aimTarget);

				velocity = vectorRotate(Vector2l{averageAcceleration * traveledTime, 0}, angle);
				velocity += startVelocity;
				
//				acceleration.set(lerp(startAcceleration, finalAcceleration, distanceTraveled, totalDistance.toLong()), 0).rotateRad(angle);
				break;
			}
			case ApproachType::BRACHISTOCHRONE: {
				//TODO implement
				throw std::runtime_error("BRACHISTOCHRONE approach not implemented");
				break;
			}
			default: {
				std::stringstream ss;
				ss << "Unknown approach type: " << approach;
				throw std::runtime_error(ss.str());
			}
		}
	}
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_TIMEDCOMPONENTS_HPP_ */
