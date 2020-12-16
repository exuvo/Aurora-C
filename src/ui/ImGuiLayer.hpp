/*
 * ImGuiLayer.h
 *
 *  Created on: Dec 16, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUILAYER_H_
#define SRC_UI_IMGUILAYER_H_

#include <imgui.h>

#include "ui/UILayer.hpp"

class ImGuiGlfw;

class ImGuiLayer: public UILayer {
	public:
		ImGuiLayer(AuroraWindow& parentWindow);
		virtual ~ImGuiLayer() override;
		virtual void render() override;
		bool eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		bool eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) override;
		bool eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		bool eventScroll(vk2d::Vector2d scroll) override;

	private:
		ImGuiContext* ctx = nullptr;
		ImGuiGlfw* imGuiGlfw = nullptr;
		
		void initShared();
};

#endif /* SRC_UI_IMGUILAYER_H_ */
