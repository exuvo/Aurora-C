/*
 * ResearchWindow.hpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#ifndef SRC_UI_IMGUI_COLONYMANAGERWINDOW_HPP_
#define SRC_UI_IMGUI_COLONYMANAGERWINDOW_HPP_

#include "UIWindow.hpp"
#include "starsystems/components/IDComponents.hpp"

struct Shipyard;
struct ShipyardSlipway;
struct ShipHull;
struct ShipyardModification;

class ColonyManagerWindow : public UIWindow {
	public:
		ColonyManagerWindow(ImGuiLayer& layer): UIWindow(layer) { visible = true; };
		virtual ~ColonyManagerWindow() = default;
		
		virtual void render() override;
		
	private:
		EntityReference selectedColony;
		Shipyard* selectedShipyard = nullptr;
		ShipyardSlipway* selectedSlipway = nullptr;
		ShipHull* selectedRetoolHull = nullptr;
		ShipHull* selectedBuildHull = nullptr;
		ShipyardModification* selectedShipyardModification = nullptr;
		uint16_t shipyardExpandCapacity = 0;
};

#endif /* SRC_UI_IMGUI_COLONYMANAGERWINDOW_HPP_ */
