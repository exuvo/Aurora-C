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
class KeyActions_StarSystemLayer;

class StarSystemLayer: public UILayer {
	public:
		StarSystemLayer(AuroraWindow& parentWindow, StarSystem* starSystem);
		virtual ~StarSystemLayer() override;
		
		float zoom = 5E5; // 1E6
		constexpr static float maxZoom = 1E8;
		int zoomLevel;
		Vector2l viewOffset {0,0}; // in m
		
		virtual void render() override;
		
		virtual bool eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) override;
		virtual bool eventScroll(vk2d::Vector2d scroll) override;
		
	private:
		StarSystem* starSystem;
		ProfilerEvents profilerEvents;
		
		void drawEntities();
		void drawSpatialPartitioning();
		void drawSpatialPartitioningPlanetoids();
		
		bool keyAction(KeyActions_StarSystemLayer action);
		
		bool inStrategicView(entt::entity entity, CircleComponent& circle);
		int getCircleSegments(float radius);
		Vector2l toWorldCoordinates(Vector2i screenCordinates);
		Matrix2l toWorldCoordinates(Matrix2i screenCordinates);
		Vector2i toScreenCoordinates(Vector2l gameCordinates);
		Matrix2i toScreenCoordinates(Matrix2l gameCordinates);
		Vector2i getMouseInScreenCordinates();
		
		Vector2i dragStart {};
		bool dragSelecting = false;
		bool dragSelectionPotentialStart = false;
		Matrix2i getDragSelection();
		
		bool movingWindow = false;
		void* selectedAction = nullptr;
		
		bool commandMenuPotentialStart = false;
		nanoseconds commandMenuPotentialStartTime = 0s;
		
		milliseconds lastTickrateUpdate;
		milliseconds galaxyTickrate = 0ms;
		milliseconds oldGalaxyTime;
};



#endif /* SRC_UI_STARSYSTEMLAYER_HPP_ */
