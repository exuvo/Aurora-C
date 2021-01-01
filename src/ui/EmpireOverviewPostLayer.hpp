/*
 * EmpireOverviewPostLayer.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_EMPIREOVERVIEWPOSTLAYER_HPP_
#define SRC_UI_EMPIREOVERVIEWPOSTLAYER_HPP_

#include "ui/UILayer.hpp"
#include "ui/imgui/EmpireOverviewWindow.hpp"

class EmpireOverviewPostLayer: public UILayer {
	public:
		EmpireOverviewPostLayer(AuroraWindow& parentWindow): UILayer(parentWindow) {};
		virtual ~EmpireOverviewPostLayer() override;
		
		virtual void render() override;
		
	private:
};

#endif /* SRC_UI_EMPIREOVERVIEWPOSTLAYER_HPP_ */
