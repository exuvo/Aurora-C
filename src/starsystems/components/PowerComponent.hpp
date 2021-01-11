/*
 * PowerComponent.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_POWERCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_POWERCOMPONENT_HPP_

#include "galaxy/ShipParts.hpp"
#include "utils/Utils.hpp"

struct PowerScheme {
	const bool chargeBatteryFromReactor;
	const uint8_t solarPanelPriority;
	const uint8_t batteryPriority;
	const uint8_t reactorPriority;
	
	bool operator==(const PowerScheme& o) const {
		return this == &o;
	}
	
	uint8_t getPowerTypeValue(Part* part) const {
		if (typeid(part) == typeid(SolarPanel)) {
			return solarPanelPriority;
			
		} else if (typeid(part) == typeid(Battery)) {
			return batteryPriority;
			
		} else if (typeid(part) == typeid(Reactor)) {
			return reactorPriority;
		}
		
		std::ostringstream os;
		os << "Invalid part type " << type_name(part);
		throw std::invalid_argument(os.str());
	}
};

struct PowerSchemes {
	static inline constexpr PowerScheme SOLAR_BATTERY_REACTOR { false, 1, 2, 3 };
	static inline constexpr PowerScheme SOLAR_REACTOR_BATTERY { true, 1, 3, 2 };
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_POWERCOMPONENT_HPP_ */
