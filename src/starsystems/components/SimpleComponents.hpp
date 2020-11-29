/*
 * SimpleComponents.hpp
 *
 *  Created on: 29 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_

#include <cstdint>

#include "galaxy/Empire.hpp"

struct TextComponent {
		char text[50];
};

struct NameComponent {
		std::string name;
};

struct TintComponent {
//TODO		Color color;
		int color;
};

struct RenderComponent {};

struct CircleComponent {
		float radius = 1;
};

struct StarSystemComponent {
		StarSystem* starSystem;
};

struct EmpireComponent {
		Empire* empire;
};

struct MassComponent {
		double mass = 0;
};

struct SunComponent {
		int32_t solarConstant = 1361;
};

struct SolarIrradianceComponent {
		int32_t irradiance = 0;
};

struct TimedLifeComponent {
		uint64_t endTime; 
};

struct AsteroidComponent {};

#endif /* SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_ */
