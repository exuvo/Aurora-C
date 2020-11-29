/*
 * Systems.hpp
 *
 *  Created on: 22 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_
#define SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_

#include <chrono>

#include <exception>
#include "entt/entt.hpp"
#include "log4cxx/logger.h"

#include "starsystems/components/Components.hpp"
#include "starsystems/systems/Scheduler.hpp"
#include "utils/GameServices.hpp"

template<typename Derived>
class BaseSystem : public Process<Derived, uint32_t> {
	public:
		BaseSystem(StarSystem* starSystem): 
			galaxy(*(starSystem->galaxy)),
			starSystem(*starSystem),
			registry(starSystem->registry)
		{};
		
	protected:
		Galaxy& galaxy;
		StarSystem& starSystem;
		entt::registry& registry;
};

template<typename Derived>
class DailySystem : public BaseSystem<Derived> {
	public:
		DailySystem(uint32_t interval, StarSystem* starSystem) : DailySystem::BaseSystem(starSystem) {
			this->interval = interval;
			lastDay = this->galaxy.day;
		}
		bool isActive() {
			if (this->galaxy.day - lastDay >= interval) {
				lastDay = this->galaxy.day;
				return true;
			}
			return false;
		}
	private:
		uint32_t lastDay;
		uint32_t interval;
};

template<typename Derived>
class IntervalSystem : public BaseSystem<Derived> {
	public:
		IntervalSystem(seconds interval, StarSystem* starSystem): IntervalSystem::BaseSystem(starSystem) {
			this->interval = interval.count();
			lastTime = this->galaxy.time;
		}
		bool isActive() {
			if (this->galaxy.time - lastTime >= interval) {
				lastTime = this->galaxy.time;
				return true;
			}
			return false;
		}
	private:
		uint64_t lastTime;
		uint32_t interval;
};

class MovementPreSystem : public IntervalSystem<MovementPreSystem> {
	public:
		MovementPreSystem(StarSystem* starSystem) : MovementPreSystem::IntervalSystem(1s, starSystem) {};
		
		void init();
		void update(delta_type delta);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.movement.pre");
};

class MovementSystem : public IntervalSystem<MovementSystem> {
	public:
		MovementSystem(StarSystem* starSystem) : MovementSystem::IntervalSystem(1s, starSystem) {};
		
		void init();
		void update(delta_type delta);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.movement");
		void moveTo(entt::entity entity, delta_type delta, TimedMovementComponent& movement, MassComponent& massComponent, ThrustComponent& thrustComponent, Vector2l targetPos, TimedMovementComponent* targetMovement, entt::entity targetEntity, ApproachType approach);
};

#endif /* SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_ */
