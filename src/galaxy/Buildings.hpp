/*
 * Buildings.hpp
 *
 *  Created on: Sep 9, 2021
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_BUILDINGS_HPP_
#define SRC_GALAXY_BUILDINGS_HPP_

#include <vector>

#include "starsystems/components/ColonyComponents.hpp"

struct TerrestrialBuildings {
	static inline TerrestrialBuilding Shipyard { "Terrestrial Shipyard" };
	static inline TerrestrialBuilding ResearchLab { "Research laboratory" };
	static inline TerrestrialBuilding DesalinationPlant { "Desalination plant" };
	static inline TerrestrialBuilding MatterConverter { "Matter converter" };
	
	static inline std::vector<TerrestrialBuilding*> ALL { &Shipyard, &ResearchLab, &DesalinationPlant,
	                                                      &MatterConverter
	};
};

struct OrbitalBuildings {
	static inline OrbitalBuilding Shipyard { "Orbital Shipyard" };
	static inline OrbitalBuilding Solarcells { "Solar cells" };
	
	static inline std::vector<OrbitalBuilding*> ALL { &Shipyard, &Solarcells };
};

#endif /* SRC_GALAXY_BUILDINGS_HPP_ */
