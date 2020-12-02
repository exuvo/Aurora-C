/*
 * SimpleComponents.hpp
 *
 *  Created on: 29 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_

#include <cstdint>

#include "utils/Utils.hpp"

struct Empire;

struct TextComponent {
		char text[50]; // not NULL terminated when full size is used
		
		TextComponent(){
			text[0] = '\0';
		};
		
		TextComponent(const char* str) {
			strncpy(text, str, ARRAY_LEN(text));
		}
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
