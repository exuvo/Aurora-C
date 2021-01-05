/*
 * StarSystemLayer.hpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */

#pragma once

#include "StarSystemLayer.hpp"

class StarSystem;
struct CircleComponent;
class KeyActions_StarSystemLayer;

class StarSystemDebugLayer: public StarSystemLayer {
	public:
		StarSystemDebugLayer(AuroraWindow& parentWindow);
		virtual ~StarSystemDebugLayer() override;
		
		virtual void render() override;
		
	private:
		void drawSpatialPartitioning();
		void drawSpatialPartitioningPlanetoids();
};
