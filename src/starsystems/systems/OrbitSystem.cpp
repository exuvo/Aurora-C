/*
 * OrbitSystem.cpp
 *
 *  Created on: Dec 29, 2020
 *      Author: exuvo
 */

#include "Aurora.hpp"
#include "Systems.hpp"
#include "utils/Math.hpp"

constexpr double gravitationalConstant = 6.67408e-11;

void OrbitSystem::init(void* data) {
	Systems* systems = (Systems*) data;
//	LOG4CXX_INFO(log, "init");
	
	registry.on_construct<OrbitComponent>().connect<&OrbitSystem::inserted>(this);
	
	registry.on_destroy<OrbitComponent>().connect<&OrbitSystem::removed>(this);
}

void OrbitSystem::inserted(entt::registry& registry, entt::entity entity) {
	addedEntites.push_back(entity);
}

void OrbitSystem::removed(entt::registry& registry, entt::entity entity) {
	removedEntites.push_back(entity);
}

void OrbitSystem::update(delta_type delta) {
//	LOG4CXX_INFO(log, "update");
	
	for (entt::entity entityID : addedEntites) {
//		std::cout << "inserted " << entityID << std::endl;
		OrbitComponent& orbit = registry.get<OrbitComponent>(entityID);
		MassComponent& parentMass = registry.get<MassComponent>(orbit.parent);
		
		if (orbit.e_eccentricity > 0.95) {
			LOG4CXX_WARN(log, "Orbital eccentricity over 0.95 won't work: " << orbit.e_eccentricity);
		}
		
		// In km
		double a_semiMajorAxis = Units::AU * orbit.a_semiMajorAxis;
		double periapsis = a_semiMajorAxis * (1.0 - orbit.e_eccentricity);
		double apoapsis = a_semiMajorAxis * (1.0 + orbit.e_eccentricity);

		// In seconds
		double orbitalPeriod = 2 * std::numbers::pi * std::sqrt(std::pow(1000.0 * a_semiMajorAxis, 3.0) / (parentMass.mass * gravitationalConstant));

		if (std::isnan(orbitalPeriod) || orbitalPeriod < 1 * 60 * 60) {
			std::ostringstream out;
			out << "orbitalPeriod " << orbitalPeriod << "s is invalid for entityID " << entityID;
			throw std::runtime_error(out.str());
		}

		// 1 point each day
		uint32_t points = std::min(std::max((int)(orbitalPeriod / (24 * 60 * 60)), 5), 1000);
		LOG4CXX_DEBUG(log, "Calculating orbit for new entity " << entityID << " using " << points << " points, orbitalPeriod " << orbitalPeriod / (24 * 60 * 60) << " days");
		std::vector<Vector2l> orbitPoints = std::vector<Vector2l>(points);

		// If set more dots represent higher speed, else the time between dots is constant
		double invert = Aurora.settings.systems.orbits.dotsRepresentSpeed ? std::numbers::pi : 0.0;

		for (uint32_t i=0; i<points; i++) {
			double M_meanAnomaly = orbit.M_meanAnomaly + (360.0 * i) / points;
			double E_eccentricAnomaly = calculateEccentricAnomalyFromMeanAnomaly(orbit, M_meanAnomaly) + invert;
			orbitPoints[i] = calculateOrbitalPositionFromEccentricAnomaly(orbit, E_eccentricAnomaly);

//			println("Calculated $i with M_meanAnomaly $M_meanAnomaly")
		}
		
		orbitsCache[entityID] = OrbitCache(orbitalPeriod, apoapsis, periapsis, std::move(orbitPoints));

		auto moonsSetIt = moonsCache.find(orbit.parent);
		std::unordered_set<entt::entity>* moonsSet;
		
		if (moonsSetIt == moonsCache.end()) {
			moonsSet = &(moonsCache[orbit.parent] = std::unordered_set<entt::entity>());
		} else {
			moonsSet = &moonsSetIt->second;
		}

		moonsSet->insert(entityID);
		
		TimedMovementComponent& movement = registry.get<TimedMovementComponent>(entityID);
		update(entityID, orbit, movement);
	}
	addedEntites.clear();
	
	for (entt::entity entity : removedEntites) {
//		std::cout << "removed " << entity << std::endl;
		orbitsCache.erase(entity);

		OrbitComponent& orbit = registry.get<OrbitComponent>(entity);

		auto moonsSet = moonsCache.find(orbit.parent);
		if (moonsSet != moonsCache.end()) {
			moonsSet->second.erase(entity);
		}
	}
	removedEntites.clear();
	
	auto view = registry.view<TimedMovementComponent, OrbitComponent>();
	
	for (entt::entity entity : view) {
		TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
		OrbitComponent& orbit = view.get<OrbitComponent>(entity);
		
		update(entity, orbit, movement);
	}
}

void OrbitSystem::update(entt::entity entityID, OrbitComponent& orbit, TimedMovementComponent& movement) {
	uint64_t today = galaxy.time;
	uint64_t dayLength = 24 * 60 * 60;
	uint64_t tomorrow = today + dayLength;
	
	OrbitCache& orbitCache = orbitsCache[entityID];
	double orbitalPeriod = orbitCache.orbitalPeriod;

	double M_meanAnomalyToday =    orbit.M_meanAnomaly + 360 * ((today % (uint64_t) orbitalPeriod) / orbitalPeriod);
	double M_meanAnomalyTomorrow = orbit.M_meanAnomaly + 360 * ((tomorrow % (uint64_t) orbitalPeriod) / orbitalPeriod);

//		println("M_meanAnomaly $M_meanAnomaly")

	double E_eccentricAnomalyToday =    calculateEccentricAnomalyFromMeanAnomaly(orbit, M_meanAnomalyToday);
	double E_eccentricAnomalyTomorrow = calculateEccentricAnomalyFromMeanAnomaly(orbit, M_meanAnomalyTomorrow);
	
	// Today
	Vector2l relativePosition = calculateOrbitalPositionFromEccentricAnomaly(orbit, E_eccentricAnomalyToday);
	relativePosition *= 1000; // km to m

	entt::entity parentEntity = orbit.parent;
	TimedMovementComponent& parentMovement = registry.get<TimedMovementComponent>(parentEntity);
	TimedValue<MovementValues> parentMovementToday = parentMovement.get(today);
	Vector2l parentPosition = parentMovementToday.value.position;
	
	Vector2l& positionToday = movement.previous.value.position;
	
	positionToday = parentPosition + relativePosition;
	
	movement.previous.time = today;
	
	// Tomorrow
	relativePosition = calculateOrbitalPositionFromEccentricAnomaly(orbit, E_eccentricAnomalyTomorrow);
	relativePosition *= 1000; // km to m
	
	TimedValue<MovementValues> parentMovementTomorrow = parentMovement.get(tomorrow);
	parentPosition = parentMovementTomorrow.value.position;
	
	Vector2l positionTomorrow = parentPosition + relativePosition;
	
	Vector2l newVelocity = positionTomorrow - positionToday;
	newVelocity = (newVelocity.cast<double>() * 100.0 / interval).cast<int64_t>();
	movement.previous.value.velocity = newVelocity;
	
	movement.setPrediction(MovementValues(positionTomorrow, newVelocity, Vector2l()), tomorrow);
	
	starSystem.changed<TimedMovementComponent>(entityID);
}

double OrbitSystem::calculateEccentricAnomalyFromMeanAnomaly(OrbitComponent& orbit, double M_meanAnomaly) {
	// Calculating orbits https://space.stackexchange.com/questions/8911/determining-orbital-position-at-a-future-point-in-time
	double M_meanAnomalyRad = toRadians(M_meanAnomaly);

	// Solve numerically using Newtons method
	double E_eccentricAnomaly = M_meanAnomalyRad;
	int attempts = 0;
	while (true) {

		double dE = (E_eccentricAnomaly - orbit.e_eccentricity * std::sin(E_eccentricAnomaly) - M_meanAnomalyRad) / (1.0 - orbit.e_eccentricity * std::cos(E_eccentricAnomaly));
		E_eccentricAnomaly -= dE;

//			println("dE $dE")

		attempts++;
		if (std::abs(dE) < 1e-5) {
			break;
		} else if (attempts >= 10) {
			LOG4CXX_WARN(log, "Calculating orbital position took more than " << attempts << " attempts");
			break;
		}
	}

	return E_eccentricAnomaly;
}

Vector2l OrbitSystem::calculateOrbitalPositionFromEccentricAnomaly(OrbitComponent& orbit, double E_eccentricAnomaly) {
	// Coordinates with P+ towards periapsis
		double P = Units::AU * orbit.a_semiMajorAxis * (std::cos(E_eccentricAnomaly) - orbit.e_eccentricity);
		double Q = Units::AU * orbit.a_semiMajorAxis * std::sin(E_eccentricAnomaly) * std::sqrt(1 - std::pow(orbit.e_eccentricity, 2.0));

//		println("orbitalPeriod ${orbitalPeriod / (24 * 60 * 60)} days, E_eccentricAnomaly $E_eccentricAnomaly, P $P, Q $Q")

		Vector2l position = { P, Q };
		return vectorRotate(position, (double) orbit.w_argumentOfPeriapsis);
}

