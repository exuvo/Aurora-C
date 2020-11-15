/*
 * Empire.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_EMPIRE_HPP_
#define SRC_GALAXY_EMPIRE_HPP_

#include <boost/circular_buffer.hpp>

#include "galaxy/Commands.hpp"

class Empire {
	public:
		boost::circular_buffer<Command*> commandQueue {128}; // For local player and ai
		
	private:
};



#endif /* SRC_GALAXY_EMPIRE_HPP_ */
