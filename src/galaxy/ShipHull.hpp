/*
 * ShipHull.hpp
 *
 *  Created on: 9 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_GALAXY_SHIPHULL_HPP_
#define SRC_GALAXY_SHIPHULL_HPP_

#include <string>
#include <fmt/format.h>

#include "starsystems/components/StrategicIconComponent.hpp"

struct HullClass {
	std::string name;
	std::string code;
};

struct ShipHull {
	std::string name;
	HullClass* hullClass;
	StrategicIcon icon;
	
	std::string toString() const;
	static inline constexpr double LengthToDiameterRatio = 2.0;
};

std::ostream& operator<< (std::ostream& out, const ShipHull& hull);

template <>
struct fmt::formatter<ShipHull> {
	// Parses format specifications
	constexpr auto parse(format_parse_context& ctx) {
		auto it = ctx.begin(), end = ctx.end();
		
		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");
		
		// Return an iterator past the end of the parsed range:
		return it;
	}
	
	template<typename FormatContext>
	auto format(const ShipHull& hull, FormatContext& ctx) {
		return format_to(ctx.out(), "{}", hull.toString());
	}
};

#endif /* SRC_GALAXY_SHIPHULL_HPP_ */
