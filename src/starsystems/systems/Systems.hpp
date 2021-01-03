/*
 * Systems.hpp
 *
 *  Created on: 22 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_
#define SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_

#include <chrono>
//#include <exception>
#include <queue>
#include <deque>
//fixme no longer compiles
//#include <eigen3/unsupported/Eigen/Polynomials>

#include "entt/entt.hpp"
#include "log4cxx/logger.h"

#include "galaxy/Galaxy.hpp"
#include "starsystems/components/Components.hpp"
#include "starsystems/systems/Scheduler.hpp"
#include "utils/quadtree/QuadTreeAABB.hpp"
#include "utils/quadtree/QuadTreePoint.hpp"

using namespace log4cxx;

struct Systems;
class QuadtreePoint;
class QuadtreeAABB;

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
	protected:
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
	protected:
		uint64_t lastTime;
		uint32_t interval;
};

struct InterceptResult {
		uint64_t timeToIntercept;
		Vector2l aimPosition;
		Vector2l interceptPosition;
		Vector2l interceptVelocity;
		Vector2l relativeInterceptVelocity;
};

class WeaponSystem : public IntervalSystem<WeaponSystem> {
	public:
		WeaponSystem(StarSystem* starSystem) : WeaponSystem::IntervalSystem(1s, starSystem) {};
		
		void init(void*);
		void update(delta_type delta);
		
		std::optional<InterceptResult> getInterceptionPosition5(MovementValues shooterMovement, MovementValues targetMovement, double missileLaunchSpeed, double missileStartAcceleration, double missileEndAcceleration, double missileAccelTime);
		std::optional<InterceptResult> getInterceptionPosition4(MovementValues shooterMovement, MovementValues targetMovement, double missileLaunchSpeed, double missileAcceleration, double missileAccelTime);
		std::optional<InterceptResult> getInterceptionPosition3(MovementValues shooterMovement, MovementValues targetMovement, double missileLaunchSpeed, double missileStartAcceleration, double missileEndAcceleration);
		std::optional<InterceptResult> getInterceptionPosition2(MovementValues shooterMovement, MovementValues targetMovement, double missileLaunchSpeed, double missileAcceleration);
		std::optional<InterceptResult> getInterceptionPosition1(MovementValues shooterMovement, MovementValues targetMovement, double projectileSpeed);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.weapon");
//		Eigen::PolynomialSolver<double, 4> polynomialSolver;
};

class MovementPreSystem : public IntervalSystem<MovementPreSystem> {
	public:
		MovementPreSystem(StarSystem* starSystem) : MovementPreSystem::IntervalSystem(1s, starSystem) {};
		
		void init(void*);
		void update(delta_type delta);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.movement.pre");
};

class MovementSystem : public IntervalSystem<MovementSystem> {
	public:
		MovementSystem(StarSystem* starSystem) : MovementSystem::IntervalSystem(1s, starSystem) {};
		
		void init(void*);
		void update(delta_type delta);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.movement");
		void moveTo(entt::entity entity, delta_type delta, TimedMovementComponent& movement, MassComponent& massComponent, ThrustComponent& thrustComponent, Vector2l targetPos, MovementValues* targetMovement, entt::entity targetEntity, ApproachType approach);
		WeaponSystem* weaponSystem = nullptr;
};

class OrbitSystem : public IntervalSystem<OrbitSystem> {
	public:
		OrbitSystem(StarSystem* starSystem) : OrbitSystem::IntervalSystem(24 * 60 * 60s, starSystem) {};
		
		void init(void*);
		void update(delta_type delta);
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.orbits");
		
		struct OrbitCache {
				double orbitalPeriod;
				double apoapsis;
				double periapsis;
				std::vector<Vector2l> orbitPoints;
		};
		
		std::unordered_map<entt::entity, OrbitCache> orbitsCache;
		std::unordered_map<entt::entity, std::unordered_set<entt::entity>> moonsCache;
		std::vector<entt::entity> addedEntites;
		std::vector<entt::entity> removedEntites;
		
		void inserted(entt::registry &, entt::entity);
		void removed(entt::registry &, entt::entity);
		void update(entt::entity, OrbitComponent& orbit, TimedMovementComponent& movement);
		double calculateEccentricAnomalyFromMeanAnomaly(OrbitComponent& orbit, double M_meanAnomaly);
		Vector2l calculateOrbitalPositionFromEccentricAnomaly(OrbitComponent& orbit, double E_eccentricAnomaly);
};

class SpatialPartitioningSystem : public IntervalSystem<SpatialPartitioningSystem> {
	public:
		SpatialPartitioningSystem(StarSystem* starSystem);
		
		void init(void*);
		void update(delta_type delta);
		static SmallList<entt::entity> query(QuadtreePoint& quadTree, Matrix2l worldCoordinates);
		
		static constexpr int32_t SCALE = 2000; // in m , min 1000
		static constexpr int32_t MAX = std::numeric_limits<int32_t>::max();
		static constexpr int64_t DESIRED_MIN_SQUARE_SIZE = 100'000'000; // in m
		static constexpr double RAW_DEPTH = std::log(SCALE * (double) MAX / DESIRED_MIN_SQUARE_SIZE) / std::log(2.0);
		static constexpr int32_t DEPTH = std::round(RAW_DEPTH);
		static constexpr int64_t MIN_SQUARE_SIZE = (SCALE * (long) MAX) / std::pow(2, DEPTH);
		static constexpr int32_t MAX_ELEMENTS = 8;
		
		QuadtreePoint tree = {MAX, MAX, MAX_ELEMENTS, DEPTH};
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.spatialpartitioning");
		
		struct comparator {
				comparator(SpatialPartitioningSystem* parent): registry(parent->registry) {};
				entt::registry& registry;
				bool operator() (entt::entity, entt::entity) const;
		};
		
		std::priority_queue<entt::entity, std::deque<entt::entity>, comparator> updateQueue;
		std::priority_queue<entt::entity, std::deque<entt::entity>, comparator> removedQueue;
		
		entt::observer accelerateObserver;
		std::vector<entt::entity> addedEntites;
		std::vector<entt::entity> removedEntites;
		
		void inserted(entt::registry &, entt::entity);
		void removed(entt::registry &, entt::entity);
		void update(entt::entity);
		uint64_t updateNextExpectedUpdate(entt::entity, MovementValues&);
};

class SpatialPartitioningPlanetoidsSystem : public IntervalSystem<SpatialPartitioningPlanetoidsSystem> {
	public:
		SpatialPartitioningPlanetoidsSystem(StarSystem* starSystem);
		
		void init(void*);
		void update(delta_type delta);
		static SmallList<entt::entity> query(QuadtreeAABB& quadTree, Matrix2l worldCoordinates);
		
		static constexpr int32_t SCALE = 2000; // in m , min 1000
		static constexpr int32_t MAX = std::numeric_limits<int32_t>::max();
		static constexpr int64_t DESIRED_MIN_SQUARE_SIZE = 149597870700; // AU in m
		static constexpr double RAW_DEPTH = std::log(SCALE * (double) MAX / DESIRED_MIN_SQUARE_SIZE) / std::log(2.0);
		static constexpr int32_t DEPTH = std::round(RAW_DEPTH); // 5
		static constexpr int64_t MIN_SQUARE_SIZE = (SCALE * (long) MAX) / std::pow(2, DEPTH);
		static constexpr int32_t MAX_ELEMENTS = 4;
		
		QuadtreeAABB tree = {MAX, MAX, MAX_ELEMENTS, DEPTH};
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.starsystems.systems.spatialpartitioningplanetoids");
		
		struct comparator {
				comparator(SpatialPartitioningPlanetoidsSystem* parent): registry(parent->registry) {};
				entt::registry& registry;
				bool operator() (entt::entity, entt::entity) const;
		};
		
		std::priority_queue<entt::entity, std::deque<entt::entity>, comparator> updateQueue;
		std::priority_queue<entt::entity, std::deque<entt::entity>, comparator> removedQueue;
		
		std::vector<entt::entity> addedEntites;
		std::vector<entt::entity> removedEntites;
		
		void inserted(entt::registry &, entt::entity);
		void removed(entt::registry &, entt::entity);
		void update(entt::entity);
		uint64_t updateNextExpectedUpdate(entt::entity, MovementValues&, CircleComponent circle);
};

struct Systems {
		WeaponSystem* weaponSystem = nullptr;
		MovementPreSystem* movementPreSystem = nullptr;
		MovementSystem* movementSystem = nullptr;
		SpatialPartitioningSystem* spatialPartitioningSystem = nullptr;
		SpatialPartitioningPlanetoidsSystem* spatialPartitioningPlanetoidsSystem = nullptr;
};

#endif /* SRC_STARSYSTEMS_SYSTEMS_SYSTEMS_HPP_ */
