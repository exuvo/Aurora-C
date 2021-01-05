/*
 * UILayer.h
 *
 *  Created on: Dec 16, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_UILAYER_H_
#define SRC_UI_UILAYER_H_

#include <ui/AuroraWindow.hpp>

class UILayer {
	public:
		UILayer(AuroraWindow& parentWindow);
		virtual ~UILayer();
		virtual void render() = 0;
		
		// Do note that cout does not work reliably in some of these for some unknown reason. log and printf does (do flush).
		virtual bool eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) { return false; };
		virtual bool eventScroll(vk2d::Vector2d scroll) { return false; };
		virtual bool eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) { return false; };
		virtual bool eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) { return false; };
		virtual void eventResized() {};
		
	protected:
		LoggerPtr log = Logger::getLogger("aurora.ui.layer");
		AuroraWindow& window;
};

#endif /* SRC_UI_UILAYER_H_ */
