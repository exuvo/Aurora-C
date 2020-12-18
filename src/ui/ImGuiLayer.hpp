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
#include "ui/UIWindow.hpp"

template<typename T>
concept aUIWindow = std::is_base_of<UIWindow, T>::value;

class ImGuiGlfw;

class ImGuiLayer: public UILayer {
	public:
		ImGuiLayer(AuroraWindow& parentWindow);
		virtual ~ImGuiLayer() override;
		
		virtual void render() override;
		void addWindow(UIWindow* uiWindow);
		void removeWindow(UIWindow* uiWindow);
		
		template<aUIWindow T>
		void showWindow();
		
		template<aUIWindow T>
		void hideWindow();
		
		virtual bool eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventScroll(vk2d::Vector2d scroll) override;

	private:
		ImGuiContext* ctx = nullptr;
		ImGuiGlfw* imGuiGlfw = nullptr;
		std::vector<UIWindow*> uiWindows;
		
		void initShared();
};

#endif /* SRC_UI_IMGUILAYER_H_ */
