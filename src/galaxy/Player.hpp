/*
 * Player.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_PLAYER_HPP_
#define SRC_GALAXY_PLAYER_HPP_

#include <string>
#include <chrono>
#include <vector>
#include <unordered_set>

#include <log4cxx/logger.h>

#include "starsystems/components/IDComponents.hpp"

using namespace std::chrono;
using namespace log4cxx;

class StarSystem;
class Empire;

class Player {
	public:
		Player(std::string name): name(name) {};
		
		std::string name;
		Empire* empire = nullptr;
		std::vector<int32_t> speedSteps = {1, 4, 10, 50, 200, 1000, 5000, 25000, 60000, 180000, 500000, 1000000, 2000000, 5000000, 10000000};
		uint8_t speedIndex = 0;
		int32_t requestedSpeed = (duration_cast<nanoseconds>(1s) / speedSteps[speedIndex]).count();
		std::unordered_set<EntityReference, EntityReference::hasher> selection;
		std::vector<StarSystem*> visibleSystems;
		static Player* current;

		void increaseSpeed();
		void decreaseSpeed();
		void pauseSpeed();
		
	private:
		LoggerPtr log = Logger::getLogger("aurora.galaxy.player");
		
	public:
};


#endif /* SRC_GALAXY_PLAYER_HPP_ */
