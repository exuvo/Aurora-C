/*
 * MunitionComponents.hpp
 *
 *  Created on: Dec 26, 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_MUNITIONCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_MUNITIONCOMPONENTS_HPP_

struct SimpleMunitionHull;
struct AdvancedMunitionHull;

struct LaserShotComponent {
		entt::entity targetEntity;
		uint64_t damage; // J per 1m² at hit distance
		uint64_t beamArea; // cm² total beam area at hit distance
		
		LaserShotComponent(entt::entity targetEntity,
		                   uint64_t damage,
		                   double beamArea // m²
		): targetEntity(targetEntity), damage(damage), beamArea(1000000 * beamArea) {}
};

struct RailgunShotComponent {
		entt::entity targetEntity;
		SimpleMunitionHull* hull;
};

struct MissileComponent {
		entt::entity targetEntity;
		AdvancedMunitionHull* hull;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_MUNITIONCOMPONENTS_HPP_ */
