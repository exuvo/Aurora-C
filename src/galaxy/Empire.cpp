/*
 * Empire.cpp
 *
 *  Created on: Jan 23, 2022
 *      Author: exuvo
 */

#include "Empire.hpp"
#include "galaxy/ShipHull.hpp"

Empire::Empire(std::string aName) {
	name = aName;
	
	hullClasses.push_back({"Dreadnought", "BA"});
	hullClasses.push_back({"Battleship", "BB"});
	hullClasses.push_back({"Battlecruiser", "BC"});
	hullClasses.push_back({"Heavy Cruiser", "CA"});
	hullClasses.push_back({"Light Cruiser", "CL"});
	hullClasses.push_back({"Command Cruiser", "CC"});
	hullClasses.push_back({"Carrier", "CV"});
	hullClasses.push_back({"Destroyer", "DA"});
	hullClasses.push_back({"Frigate", "DB"});
	hullClasses.push_back({"Corvette", "DC"});
	hullClasses.push_back({"Fighter", "FF"});
	
	// Logistics
	hullClasses.push_back({"Collier", "LC"});
	hullClasses.push_back({"Ammunition Transport", "LA"});
	hullClasses.push_back({"Combat Stores Transport", "LS"});
	hullClasses.push_back({"Tanker", "LT"});
	hullClasses.push_back({"Construction Ship", "LB"});
	hullClasses.push_back({"Colony Ship", "LE"});
	hullClasses.push_back({"Freighter", "LF"});
	
//	hullClasses.sort { hull1, hull2 -> hull1.code.compareTo(hull2.code) };
};
