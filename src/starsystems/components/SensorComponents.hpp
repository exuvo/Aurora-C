/*
 * SensorComponents.hpp
 *
 *  Created on: Jan 9, 2021
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_SENSORCOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_SENSORCOMPONENTS_HPP_

#include <string>
#include "utils/Utils.hpp"

struct Spectrum {
	char name[8];
	
	bool operator==(const Spectrum& o) const {
		return this == &o;
	}
};

struct Spectra {
	static inline constexpr Spectrum Visible_Light { "L" };
	static inline constexpr Spectrum Electromagnetic { "EM" };
	static inline constexpr Spectrum Thermal { "Thermal" };
	
	static inline constexpr Spectrum ALL[] { Visible_Light, Electromagnetic, Thermal };
};

#endif /* SRC_STARSYSTEMS_COMPONENTS_SENSORCOMPONENTS_HPP_ */
