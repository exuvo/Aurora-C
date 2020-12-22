/*
 * KeyMappings.hpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_KEYMAPPINGS_HPP_
#define SRC_UI_KEYMAPPINGS_HPP_

#include <GLFW/glfw3.h>
#include <VK2D.h>

#include "utils/enum.h"

enum class KeyPressType {
	Keyboard,
	Character,
	Ignore
};

struct KeyMapping {
		std::string name;
		KeyPressType type;
		int32_t key;
};

BETTER_ENUM(KeyActions_StarSystemLayer, uint8_t,
    NONE,
		SPEED_UP,
		SPEED_DOWN,
		GENERATE_SYSTEM,
		PAUSE,
		MAP,
		ATTACK
);

BETTER_ENUM(KeyActions_ImGuiLayer, uint8_t,
    NONE,
		DEBUG,
		SHIP_DEBUG,
		COLONY_MANAGER,
		SHIP_DESIGNER,
		RESEARCH,
		PROFILER
);

//class KeyActions_ImGuiLayer {
//		KeyMapping mappings[] = {
//		  { "IMGUI_DEBUG", KeyPressType::Keyboard, GLFW_KEY_GRAVE_ACCENT },
//		};
//};

template<typename T>
struct KeyMappingRaw {
		T keyAction;
		int32_t scancode; // raw keycode from OS
		uint32_t keyCode; // ex GLFW_KEY_M
};

template<typename T>
struct KeyMappingTranslated {
		T keyAction;
		uint32_t keyCode; // ex GLFW_KEY_M
		vk2d::ModifierKeyFlags modifier_keys;
		uint32_t character; // used if scancode is -1;
};

class KeyMappings {
	public:
		template<typename T>
		static T getRaw(int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys);

		//TODO glfw character callback is not actually translated https://github.com/glfw/glfw/issues/1502
		template<typename T>
		static T getTranslated(uint32_t character);
		
		template<typename T>
		static void loadDefaults();
		
		static void loadAllDefaults();
//		static void load();
//		static void save();

	private:
};


#endif /* SRC_UI_KEYMAPPINGS_HPP_ */
