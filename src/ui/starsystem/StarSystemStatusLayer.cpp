/*
 * StarSystemStatusLayer.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */
#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include "Aurora.hpp"
#include "StarSystemStatusLayer.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/ShadowStarSystem.hpp"
#include "starsystems/components/Components.hpp"
#include "galaxy/Galaxy.hpp"
#include "ui/KeyMappings.hpp"

StarSystemStatusLayer::StarSystemStatusLayer(AuroraWindow& parentWindow, StarSystemInputLayer& inputLayer)
: StarSystemLayer(parentWindow),
  inputLayer(inputLayer)
{
	lastTickrateUpdate = getMillis();
	oldGalaxyTime = milliseconds(Aurora.galaxy->time);
}

StarSystemStatusLayer::~StarSystemStatusLayer() {
}

__attribute__((always_inline))
static inline void writeText(AuroraWindow& window, float& x, float& y, std::string text, vk2d::Colorf color = vk2d::Colorf::WHITE()) {
		vk2d::Mesh& text_mesh = RenderCache::getTextMeshCallerCentric(Aurora.assets.font, { std::floor(x), std::floor(y) }, text, color);
		window.window->DrawMesh(text_mesh);
//		window.window->DrawRectangle(text_mesh.aabb, false, vk2d::Colorf::OLIVE());
		x += text_mesh.aabb.GetAreaSize().x;
//		std::cout << "size " << text_mesh.aabb << " " << text_mesh.aabb.GetAreaSize() << std::endl;
}

void StarSystemStatusLayer::render() {
	if (inputLayer.dragSelecting) {
		Matrix2i selection = inputLayer.getDragSelection();
		window.window->DrawRectangle(matrixToVK2D(selection), false, vk2d::Colorf::WHITE());
	}
	
	if (inputLayer.tracking) {
		vk2d::Mesh& text_mesh = RenderCache::getTextMeshCallerCentric(Aurora.assets.font, { window.window->GetSize().x / 2 - 50, window.window->GetSize().y - 30 }, "Tracking");
		window.window->DrawMesh(text_mesh);
	}
	
	milliseconds now = getMillis();
		
	if (now - lastTickrateUpdate > 1000ms) {
		lastTickrateUpdate = now;
		galaxyTickrate = milliseconds(Aurora.galaxy->time) - oldGalaxyTime;
		oldGalaxyTime = milliseconds(Aurora.galaxy->time);
	}
	
	float y = window.window->GetSize().y - 10;
	float x = 5;
	
//	std::cout << std::endl;
	writeText(window, x, y, fmt::format("{} {}  ", daysToDate(Aurora.galaxy->day), secondsToString(Aurora.galaxy->time)));
	
	if (Aurora.galaxy->speed == 0ns) {
		writeText(window, x, y, "System Error", vk2d::Colorf::RED());
		
	} else if (Aurora.galaxy->speed.count() < 0) {
		writeText(window, x, y, fmt::format("speed {}", Units::NANO_SECOND / -Aurora.galaxy->speed.count()), vk2d::Colorf::GREY());
		
	} else if (Aurora.galaxy->speedLimited) {
		writeText(window, x, y, fmt::format("speed {}", Units::NANO_SECOND / Aurora.galaxy->speed.count()), vk2d::Colorf::RED());
		
	}  else {
		writeText(window, x, y, fmt::format("speed {}", Units::NANO_SECOND / Aurora.galaxy->speed.count()));
	}
	
	writeText(window, x, y, fmt::format(" {}", Aurora.galaxy->tickSize));
	writeText(window, x, y, fmt::format(" {}us {}t/s", (int)(starSystem->updateTimeAverage / 1000), galaxyTickrate.count()));
	writeText(window, x, y, fmt::format(", {}st", starSystem->registry.alive()));
	
	std::string text = fmt::format("zoom {:02}", inputLayer.zoomLevel);
	vk2d::Vector2f bb = Aurora.assets.font->CalculateRenderedSize(text).GetAreaSize();
	window.window->DrawMesh(RenderCache::getTextMeshCallerCentric(Aurora.assets.font, { window.window->GetSize().x - bb.x - 4 , y }, text));
}

