/*
 * StrategicIconComponent.hpp
 *
 *  Created on: Dec 18, 2020
 *      Author: exuvo
 */

#ifndef SRC_STARSYSTEMS_COMPONENTS_STRATEGICICONCOMPONENT_HPP_
#define SRC_STARSYSTEMS_COMPONENTS_STRATEGICICONCOMPONENT_HPP_

//TODO implement
class TextureRegion;

struct StrategicIconComponent {
	TextureRegion* baseTexture;
	TextureRegion* centerTexture;
	
	StrategicIconComponent(TextureRegion* icon) {
		baseTexture = icon;
		centerTexture = nullptr;
	}
	
	StrategicIconComponent(TextureRegion* baseIcon, TextureRegion* centerIcon) {
		baseTexture = baseIcon;
		centerTexture = centerIcon;
	}
};



#endif /* SRC_STARSYSTEMS_COMPONENTS_STRATEGICICONCOMPONENT_HPP_ */
