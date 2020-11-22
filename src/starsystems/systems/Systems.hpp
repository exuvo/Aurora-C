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

#include "starsystems/systems/scheduler.hpp"
#include "utils/GameServices.hpp"

template<typename Derived>
class BaseSystem : public process<Derived, uint32_t> {
	public:
		BaseSystem(StarSystem* starSystem) {
			this->starSystem = starSystem;
			galaxy = starSystem->galaxy;
		}
	protected:
		StarSystem* starSystem;
		Galaxy* galaxy;
};

template<typename Derived>
class DailySystem : public BaseSystem<Derived> {
	using DailySystem::BaseSystem::galaxy;
	
	public:
		DailySystem(uint32_t interval, StarSystem* starSystem) : DailySystem::BaseSystem(starSystem) {
			this->interval = interval;
			lastDay = galaxy->day;
		}
		bool checkProcessing() {
			if (galaxy->day - lastDay >= interval) {
				lastDay = galaxy->day;
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
	using IntervalSystem::BaseSystem::galaxy;
	
	public:
		IntervalSystem(seconds interval, StarSystem* starSystem): IntervalSystem::BaseSystem(starSystem) {
			this->interval = interval.count();
			lastTime = galaxy->time;
		}
		bool checkProcessing() {
			if (galaxy->time - lastTime >= milliseconds(interval)) {
				lastTime = galaxy->time;
				return true;
			}
			return false;
		}
	private:
		seconds lastTime;
		uint32_t interval;
};

class MovementSystem : public IntervalSystem<MovementSystem> {
	public:
		MovementSystem(StarSystem* starSystem) : MovementSystem::IntervalSystem(1s, starSystem) {
		};
		
		void init(void* data);
		void update(delta_type delta);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.movement");
};

#endif /* SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_ */
