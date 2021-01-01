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
#include "ui/imgui/UIWindow.hpp"
#include "utils/Utils.hpp"

template<typename T>
concept aUIWindow = std::is_base_of<UIWindow, T>::value;

class ImGuiGlfw;
class KeyActions_ImGuiLayer;

class ImGuiLayer: public UILayer {
	public:
		ImGuiLayer(AuroraWindow& parentWindow);
		virtual ~ImGuiLayer() override;
		
		virtual void render() override;
		void addWindow(UIWindow* uiWindow);
		void removeWindow(UIWindow* uiWindow);
		
		AuroraWindow& getMainWindow() {
			return window;
		}
		
		template<aUIWindow T>
		T& getWindow() {
			for (UIWindow* uiWindow : uiWindows) {
				auto casted = dynamic_cast<T*>(uiWindow);
				if (casted) {
					return *casted;
				}
			}
			
			throw std::invalid_argument("No ui window of type " + type_name<T>());
		}
		
		template<aUIWindow T>
		void showWindow() {
			getWindow<T>().visible = true;
		}
		
		template<aUIWindow T>
		void hideWindow() {
			getWindow<T>().visible = false;
		}
		
		template<aUIWindow T>
		void toggleWindow() {
			T& window = getWindow<T>();
			window.visible = !window.visible;
		}
		
		bool keyAction(KeyActions_ImGuiLayer action);
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
