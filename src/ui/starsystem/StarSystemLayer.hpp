/*
 * StarSystemLayer.hpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */

#ifndef SRC_UI_STARSYSTEMLAYER_HPP_
#define SRC_UI_STARSYSTEMLAYER_HPP_

#include "ui/UILayer.hpp"
#include "utils/Math.hpp"
#include "utils/Profiling.hpp"
#include "utils/RenderUtils.hpp"
#include "ui/RenderCache.hpp"

class StarSystem;
struct CircleComponent;

class StarSystemLayer: public UILayer {
	public:
		StarSystemLayer(AuroraWindow& parentWindow);
		virtual ~StarSystemLayer() override;
		
		static constexpr float STRATEGIC_ICON_SIZE = 24;
		
		float& zoom;
		Vector2l& viewOffset;
		StarSystem*& starSystem;
		ProfilerEvents& profilerEvents;
		
	protected:
		bool inStrategicView(entt::entity entity, CircleComponent& circle);
		int getCircleSegments(float radius);
		Vector2l toWorldCoordinates(Vector2i screenCordinates);
		Matrix2l toWorldCoordinates(Matrix2i screenCordinates);
		Vector2i toScreenCoordinates(Vector2l gameCordinates);
		Matrix2i toScreenCoordinates(Matrix2l gameCordinates);
		Vector2i getMouseInScreenCordinates();
};

void addStarSystemLayers(AuroraWindow& window);

#endif /* SRC_UI_STARSYSTEMLAYER_HPP_ */
