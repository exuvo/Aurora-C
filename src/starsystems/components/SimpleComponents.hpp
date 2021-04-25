/*
 * SimpleComponents.hpp
 *
 *  Created on: 29 Nov 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_

#include <cstdint>
#include <Types/Color.hpp>

#include "utils/Utils.hpp"

#include "refureku/SimpleComponents.rfk.h"

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
		vk2d::Colorf color;
};

struct RenderComponent {
		char dummy; //entt requires storage in components
};

struct RFKStruct() CircleComponent {
		float radius = 1; // in m
		CircleComponent_GENERATED
};

struct StarSystemComponent {
		StarSystem* starSystem;
};

struct EmpireComponent {
		Empire* empire;
};

struct MassComponent {
		double mass = 0; // In kg
};

// W/m2 @ 1 AU. https://en.wikipedia.org/wiki/Solar_constant
struct SunComponent {
		int32_t solarConstant = 1361;
};

struct SolarIrradianceComponent {
		int32_t irradiance = 0; // W/m2
};

struct TimedLifeComponent {
		uint64_t endTime; 
};

struct AsteroidComponent {};

struct CrewComponent {
	uint16_t alive;
	uint16_t dead;
	uint8_t morale; // 100 normal
};

struct HeatComponent {
	uint32_t temperature; // In kelvin
};

File_GENERATED
#endif /* SRC_STARSYSTEMS_COMPONENTS_SIMPLECOMPONENTS_HPP_ */
