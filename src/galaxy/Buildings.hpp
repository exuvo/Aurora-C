/*
 * Buildings.hpp
 *
 *  Created on: Sep 9, 2021
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_BUILDINGS_HPP_
#define SRC_GALAXY_BUILDINGS_HPP_

#include <array>

#include "starsystems/components/ColonyComponents.hpp"

struct TerrestrialBuildings {
	static inline constexpr TerrestrialBuilding Shipyard { "TS", "Terrestrial Shipyard", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	static inline constexpr TerrestrialBuilding ResearchLab { "RL", "Research laboratory", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	static inline constexpr TerrestrialBuilding DesalinationPlant { "DP", "Desalination plant", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	static inline constexpr TerrestrialBuilding MatterConverter { "MC", "Matter converter", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	
	static inline constexpr const TerrestrialBuilding* ALL[] { &Shipyard, &ResearchLab, &DesalinationPlant, &MatterConverter };
	static inline constexpr size_t ALL_size = ARRAY_LENGTH(ALL);
};

struct OrbitalBuildings {
	static inline constexpr OrbitalBuilding Shipyard { "OSY", "Orbital Shipyard", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	static inline constexpr OrbitalBuilding Solarpanels { "SP", "Solar panels", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
	
	static inline constexpr const OrbitalBuilding* ALL[] { &Shipyard, &Solarpanels };
	static inline constexpr size_t ALL_size = ARRAY_LENGTH(ALL);
};

#endif /* SRC_GALAXY_BUILDINGS_HPP_ */
