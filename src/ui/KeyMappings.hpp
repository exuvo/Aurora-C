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

//enum class KeyActions_ImGuiLayer {
//		DEBUG(Input.Keys.GRAVE),
//		SHIP_DEBUG(Input.Keys.F1),
//		COLONY_MANAGER(Input.Keys.F2),
//		SHIP_DESIGNER(Input.Keys.F3),
//		RESEARCH(Input.Keys.F4),
//		PROFILER(Input.Keys.F5);
//};
//
//class KeyActions_ImGuiLayer {
//		KeyMapping mappings[] = {
//		  { "IMGUI_DEBUG", KeyPressType::Keyboard, GLFW_KEY_GRAVE_ACCENT },
//		};
//};

class KeyMappings {
	public:
		template<typename T>
		T getKeyboard(int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys);

		template<typename T>
		T getCharacter(uint32_t character);

	private:
};

#endif /* SRC_UI_KEYMAPPINGS_HPP_ */
