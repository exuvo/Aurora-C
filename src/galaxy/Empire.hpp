/*
 * Empire.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_EMPIRE_HPP_
#define SRC_GALAXY_EMPIRE_HPP_

#include <boost/circular_buffer.hpp>
#include <VK2D.h>

#include "galaxy/Commands.hpp"
#include "galaxy/ShipHullClass.hpp"

class Part;
class ShipHull;

class Empire {
	public:
		uint8_t id = 0;
		std::string name;
		vk2d::Colorf color = vk2d::Colorf::WHITE();
		boost::circular_buffer<Command*> commandQueue {128}; // For local player and ai
		std::vector<EntityReference> colonies;
		std::vector<EntityReference> stations;
		
		std::vector<Part*> parts;
		std::vector<ShipHull*> shipHulls;
		std::vector<HullClass> hullClasses;
		
		Empire(std::string aName);
		
//		static const Empire gaia = { "GAIA" };
		
	private:
};



#endif /* SRC_GALAXY_EMPIRE_HPP_ */
