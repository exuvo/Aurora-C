/*
 * StarSystemLayer.hpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */

#pragma once

#include "StarSystemLayer.hpp"

class StarSystem;
class KeyActions_StarSystemLayer;
class StarSystemStatusLayer;

class StarSystemInputLayer: public StarSystemLayer {
	public:
		StarSystemInputLayer(AuroraWindow& parentWindow);
		virtual ~StarSystemInputLayer() override;
		friend StarSystemStatusLayer;
		
		static constexpr float maxZoom = 1E8;
		int zoomLevel;
		
		virtual void render() override;
		
		virtual bool eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventScroll(vk2d::Vector2d scroll) override;
		
	private:
		bool keyAction(KeyActions_StarSystemLayer action);
		
		Vector2i dragStart {};
		bool dragSelecting = false;
		bool dragSelectionPotentialStart = false;
		Matrix2i getDragSelection();
		
		bool movingView = false;
		void* selectedAction = nullptr;
		
		bool commandMenuPotentialStart = false;
		nanoseconds commandMenuPotentialStartTime = 0s;
		
		bool tracking = false;
};
