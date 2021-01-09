/*
 * ShipHull.cpp
 *
 *  Created on: Jan 3, 2021
 *      Author: exuvo
 */

#include "ShipHull.hpp"

std::string ShipHull::toString() const {
	return "";
//	val parentHull = parentHull;
//	
//	if (parentHull == null) {
//		return "$name ${Units.daysToYear(designDay)}";
//	}
//	
//	return "$name ${Units.daysToYear(parentHull.designDay)}-${Units.daysToSubYear(designDay)}";
}

std::ostream& operator<< (std::ostream& out, const ShipHull& hull) {
	return out << hull.toString();
}
