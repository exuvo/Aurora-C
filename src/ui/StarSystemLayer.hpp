/*
 * StarSystemLayer.hpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_STARSYSTEMLAYER_HPP_
#define SRC_UI_STARSYSTEMLAYER_HPP_

#include "ui/UILayer.hpp"
#include "utils/Profiling.hpp"
#include "utils/Math.hpp"

class StarSystem;
struct CircleComponent;

class StarSystemLayer: public UILayer {
	public:
		StarSystemLayer(AuroraWindow& parentWindow, StarSystem* starSystem);
		virtual ~StarSystemLayer() override;
		
		float zoom = 1;
		Vector2l viewOffset {};
		
		virtual void render() override;
		
		virtual bool eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventScroll(vk2d::Vector2d scroll) override;
		
	private:
		StarSystem* starSystem;
		ProfilerEvents profilerEvents;
		
		void drawEntities();
		
		bool inStrategicView(entt::entity entity, CircleComponent& circle);
		int getCircleSegments(float radius);
};



#endif /* SRC_UI_STARSYSTEMLAYER_HPP_ */
