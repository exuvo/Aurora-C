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

#include <Refureku/NativeProperties.h>
#include "refureku/SimpleComponents.rfk.h"

struct Empire;
class StarSystem;

struct RFKStruct(ParseAllNested) TextComponent {
		char text[50]; // not NULL terminated when full size is used
		
		TextComponent(){
			text[0] = '\0';
		};
		
		TextComponent(const char* str) {
			strncpy(text, str, ARRAY_LEN(text));
		}
		
		TextComponent_GENERATED
};

struct NameComponent {
		std::string name;
};

struct RFKStruct(ParseAllNested) TintComponent {
		vk2d::Colorf color;
		TintComponent_GENERATED
};

struct RFKStruct(ParseAllNested) RenderComponent {
		char dummy; //entt requires storage in components
		RenderComponent_GENERATED
};

struct RFKStruct(ParseAllNested) CircleComponent {
		float radius = 1; // in m
		CircleComponent_GENERATED
};

struct StarSystemComponent {
		StarSystem* starSystem;
};

struct EmpireComponent {
		Empire* empire;
		
		EmpireComponent(): empire(nullptr){};
		EmpireComponent(Empire* empire): empire(empire){};
		EmpireComponent(Empire& empire): empire(&empire){};
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
