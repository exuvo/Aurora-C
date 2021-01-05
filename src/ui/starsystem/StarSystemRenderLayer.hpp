/*
 * StarSystemLayer.hpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */

#pragma once

#include "StarSystemLayer.hpp"

class StarSystem;

class StarSystemRenderLayer: public StarSystemLayer {
	public:
		StarSystemRenderLayer(AuroraWindow& parentWindow);
		virtual ~StarSystemRenderLayer() override;
		
		static constexpr float maxZoom = 1E8;
		int zoomLevel;
		
		virtual void render() override;
		
	private:
		void drawEntities();
};
