/*
 * KeyMappings.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: exuvo
 */

#include <unordered_map>
#include <boost/hana.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/seq.hpp>
#include <iostream>

#include <GLFW/glfw3.h>
#include <log4cxx/logger.h>

#include "KeyMappings.hpp"

using namespace log4cxx;

static LoggerPtr keyLog = Logger::getLogger("aurora.ui.keymappings");

#define KEY_ACTIONS_TUPLE (KeyActions_StarSystemLayer, KeyActions_ImGuiLayer) // max 25, after that write sequence directly (a)(b)(c)
#define KEY_ACTIONS_SEQ BOOST_PP_TUPLE_TO_SEQ(KEY_ACTIONS_TUPLE)
#define KEY_ACTIONS_SEQ_SIZE BOOST_PP_SEQ_SIZE(KEY_ACTIONS_SEQ)
#define KEY_ACTIONS BOOST_PP_SEQ_ENUM(KEY_ACTIONS_SEQ)

#define KEY_ACTIONS_TEMPLATE(r, unused, KeyAction) \
static std::unordered_map<int32_t, KeyMappingRaw<KeyAction>> BOOST_PP_CAT(keyRawMap_, KeyAction); \
static std::unordered_map<uint32_t, KeyMappingTranslated<KeyAction>> BOOST_PP_CAT(keyTranslatedMap_, KeyAction); \
\
template<> \
KeyAction KeyMappings::getRaw<KeyAction>(int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) { \
	if (action == vk2d::ButtonAction::PRESS) { \
		LOG4CXX_TRACE(keyLog, "raw scancode " << scancode << " " << glfwGetKeyName(GLFW_KEY_UNKNOWN, scancode)); \
		/*std::unordered_map<uint32_t, KeyMappingRaw<KeyAction>>::const_iterator it = BOOST_PP_CAT(keyRawMap_, KeyAction).find(((uint32_t) scancode & 0xFFFFFF) | (static_cast<uint32_t>(modifier_keys) & 0xFF) << 24); */ \
		std::unordered_map<int32_t, KeyMappingRaw<KeyAction>>::const_iterator it = BOOST_PP_CAT(keyRawMap_, KeyAction).find(scancode); \
		if (it != BOOST_PP_CAT(keyRawMap_, KeyAction).end()) { \
			return it->second.keyAction; \
		} \
	} \
	return KeyAction::NONE; \
} \
\
template<> \
KeyAction KeyMappings::getTranslated<KeyAction>(uint32_t character) { \
	LOG4CXX_TRACE(keyLog, "translated character " << character << " " << glfwGetKeyName(character, 0)); \
	std::unordered_map<uint32_t, KeyMappingTranslated<KeyAction>>::iterator it = BOOST_PP_CAT(keyTranslatedMap_, KeyAction).find(character); \
	if (it != BOOST_PP_CAT(keyTranslatedMap_, KeyAction).end()) { \
		return it->second.keyAction; \
	} \
	return KeyAction::NONE; \
};

//TODO no way to map from keycode back to key https://github.com/glfw/glfw/issues/1502

BOOST_PP_SEQ_FOR_EACH(KEY_ACTIONS_TEMPLATE, ~, KEY_ACTIONS_SEQ)

template<typename KeyAction>
void addRawKeyBind(std::unordered_map<int32_t, KeyMappingRaw<KeyAction>>& map, typename KeyAction::_enumerated action, uint32_t keycode) {
	int32_t scancode = glfwGetKeyScancode(keycode);
	map.insert({{ scancode, { action, scancode, keycode } }});
	LOG4CXX_TRACE(keyLog, "adding scancode " << scancode << ", key " << keycode << " " << glfwGetKeyName(keycode, 0));
}

template<>
void KeyMappings::loadDefaults<KeyActions_StarSystemLayer>() {
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::SPEED_UP, GLFW_KEY_KP_ADD);
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::SPEED_DOWN, GLFW_KEY_KP_SUBTRACT);
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::GENERATE_SYSTEM, GLFW_KEY_G);
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::PAUSE, GLFW_KEY_SPACE);
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::MAP, GLFW_KEY_M);
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::ATTACK, GLFW_KEY_A);
	addRawKeyBind(keyRawMap_KeyActions_StarSystemLayer, KeyActions_StarSystemLayer::TRACK, GLFW_KEY_T);
	
//	keyRawMap_KeyActions_StarSystemLayer.insert({
//			{GLFW_KEY_KP_ADD, KeyActions_StarSystemLayer::SPEED_UP},
//			{GLFW_KEY_KP_SUBTRACT, KeyActions_StarSystemLayer::SPEED_DOWN},
//			{GLFW_KEY_G, KeyActions_StarSystemLayer::GENERATE_SYSTEM},
//			{GLFW_KEY_SPACE, KeyActions_StarSystemLayer::PAUSE},
//			{GLFW_KEY_M, KeyActions_StarSystemLayer::MAP},
//			{GLFW_KEY_A, KeyActions_StarSystemLayer::ATTACK}
//	});
}

template<>
void KeyMappings::loadDefaults<KeyActions_ImGuiLayer>() {
	addRawKeyBind(keyRawMap_KeyActions_ImGuiLayer, KeyActions_ImGuiLayer::DEBUG, GLFW_KEY_GRAVE_ACCENT);
	addRawKeyBind(keyRawMap_KeyActions_ImGuiLayer, KeyActions_ImGuiLayer::SHIP_DEBUG, GLFW_KEY_F1);
	addRawKeyBind(keyRawMap_KeyActions_ImGuiLayer, KeyActions_ImGuiLayer::COLONY_MANAGER, GLFW_KEY_F2);
	addRawKeyBind(keyRawMap_KeyActions_ImGuiLayer, KeyActions_ImGuiLayer::SHIP_DESIGNER, GLFW_KEY_F3);
	addRawKeyBind(keyRawMap_KeyActions_ImGuiLayer, KeyActions_ImGuiLayer::RESEARCH, GLFW_KEY_F4);
	addRawKeyBind(keyRawMap_KeyActions_ImGuiLayer, KeyActions_ImGuiLayer::PROFILER, GLFW_KEY_F5);
	
//	keyRawMap_KeyActions_ImGuiLayer.insert({
//			{GLFW_KEY_GRAVE_ACCENT, KeyActions_ImGuiLayer::DEBUG},
//			{GLFW_KEY_F1, KeyActions_ImGuiLayer::SHIP_DEBUG},
//			{GLFW_KEY_F2, KeyActions_ImGuiLayer::COLONY_MANAGER},
//			{GLFW_KEY_F3, KeyActions_ImGuiLayer::SHIP_DESIGNER},
//			{GLFW_KEY_F4, KeyActions_ImGuiLayer::RESEARCH},
//			{GLFW_KEY_F5, KeyActions_ImGuiLayer::PROFILER}
//	});
}

#define LOAD_DEFAULTS_TEMPLATE(r, unused, KeyAction) loadDefaults<KeyAction>();

void KeyMappings::loadAllDefaults() {
	BOOST_PP_SEQ_FOR_EACH(LOAD_DEFAULTS_TEMPLATE, ~, KEY_ACTIONS_SEQ)
	loadDefaults<KeyActions_StarSystemLayer>();
}
