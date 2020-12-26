/*
 * ShipComponent.hpp
 *
 *  Created on: Dec 26, 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_SHIPCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_SHIPCOMPONENT_HPP_

#include <stdint.h>

struct ShipHull;

struct ShipComponent {
		ShipHull* hull;
		uint64_t commissionTime;
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_SHIPCOMPONENT_HPP_ */
