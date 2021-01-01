/*
 * Aurora.hpp
 *
 *  Created on: Dec 14, 2020
 *      Author: exuvo
 */

#ifndef SRC_AURORA_HPP_
#define SRC_AURORA_HPP_

#include <vector>

class Galaxy;
class AuroraWindow;
namespace vk2d {
	class Instance;
	class FontResource;
}

//TODO reflection load these from xml
struct Settings {
	struct {
		bool vsync = true;
		float zoomSensitivity = 1.25;
		bool debugDisableStrategicView = false;
		bool debugSpatialPartitioning = false;
		bool debugSpatialPartitioningPlanetoids = false;
		bool useShadow = false;
	} render;
	struct {
			struct {
					bool dotsRepresentSpeed = true;
			} orbits;
	} systems;
};

struct Assets {
	vk2d::FontResource* font;
	vk2d::FontResource* font5;
	vk2d::FontResource* font6;
	vk2d::FontResource* font7;
	vk2d::FontResource* font8;
	vk2d::FontResource* font9;
	vk2d::FontResource* font11;
	vk2d::FontResource* font13;
};

struct AuroraGlobal {
	Settings settings;
	Assets assets;
//	using galaxy = entt::service_locator<Galaxy>;
	Galaxy* galaxy;
	std::vector<AuroraWindow*> windows;
	bool shutdown = false;
	vk2d::Instance* vk2dInstance;
	uint16_t fps;
//		struct ImGui_ImplVulkan_InitInfo imGuiImpl; // For docking branch
};

extern AuroraGlobal Aurora;

#endif /* SRC_AURORA_HPP_ */
