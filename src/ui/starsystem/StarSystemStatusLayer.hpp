/*
 * StarSystemLayer.hpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */

#pragma once

#include "StarSystemLayer.hpp"
#include "ui/starsystem/StarSystemInputLayer.hpp"

class StarSystem;

class StarSystemStatusLayer: public StarSystemLayer {
	public:
		StarSystemStatusLayer(AuroraWindow& parentWindow, StarSystemInputLayer& inputLayer);
		virtual ~StarSystemStatusLayer() override;
		
		virtual void render() override;
		
	private:
		StarSystemInputLayer& inputLayer;
		
		milliseconds lastTickrateUpdate;
		milliseconds galaxyTickrate = 0ms;
		milliseconds oldGalaxyTime;
};
