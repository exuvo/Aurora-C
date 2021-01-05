/*
 * StarSystemLayer.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */
#include "Aurora.hpp"
#include "StarSystemLayer.hpp"
#include "ui/starsystem/StarSystemInputLayer.hpp"
#include "ui/starsystem/StarSystemRenderLayer.hpp"
#include "ui/starsystem/StarSystemDebugLayer.hpp"
#include "ui/starsystem/StarSystemStatusLayer.hpp"

void addStarSystemLayers(AuroraWindow& window){
	auto inputLayer = new StarSystemInputLayer(window);
	window.addLayer(inputLayer);
	window.addLayer(new StarSystemRenderLayer(window));
	window.addLayer(new StarSystemDebugLayer(window));
	window.addLayer(new StarSystemStatusLayer(window, *inputLayer));
}

StarSystemLayer::StarSystemLayer(AuroraWindow& parentWindow)
: UILayer(parentWindow),
  zoom(window.zoom),
  viewOffset(window.viewOffset),
  starSystem(window.starSystem),
  profilerEvents(window.profilerEvents)
{
}

StarSystemLayer::~StarSystemLayer() {
}

bool StarSystemLayer::inStrategicView(entt::entity entity, CircleComponent& circle) {
	return false;
//	if (Aurora.settings.render.debugDisableStrategicView || zoom == 1.0f) {
//		return false;
//	}
//	
//	float radius = circle.radius / 1000;
//	return radius / zoom < 5.0f;
}

// radius in screen space radius
int StarSystemLayer::getCircleSegments(float radius) {
	return std::min(1000, std::max(3, (int) (9 * std::cbrt(radius))));
}

Vector2i StarSystemLayer::getMouseInScreenCordinates() {
	vk2d::Vector2i cursor = window.mousePos;
	return { cursor.x, cursor.y };
}

Vector2l StarSystemLayer::toWorldCoordinates(Vector2i screenCoordinates) {
	Vector2d worldCoordinates = screenCoordinates.cast<double>();
	
	vk2d::Vector2u windowSize = window.window->GetSize() / 2;
	worldCoordinates -= Vector2d{ windowSize.x, windowSize.y };
	
	worldCoordinates *= 1000 * zoom; // km to m
	return worldCoordinates.cast<int64_t>() + viewOffset;
}

Matrix2l StarSystemLayer::toWorldCoordinates(Matrix2i screenCoordinates) {
	vk2d::Vector2u windowSize = window.window->GetSize() / 2;
	Matrix2i windowSizeMat;
	windowSizeMat << windowSize.x, windowSize.y, windowSize.x, windowSize.y;
	screenCoordinates -= windowSizeMat;
	
	Matrix2d worldCoordinates = screenCoordinates.cast<double>();
	worldCoordinates *= 1000 * zoom; // km to m
	Matrix2l worldCoordinates2 = worldCoordinates.cast<int64_t>();
	worldCoordinates2.row(0) += viewOffset;
	worldCoordinates2.row(1) += viewOffset;
	return worldCoordinates2;
}

Vector2i StarSystemLayer::toScreenCoordinates(Vector2l gameCordinates){
	gameCordinates -= viewOffset;
	Vector2d screenCordinates = gameCordinates.cast<double>();
	screenCordinates /= zoom * 1000; // m to km
	
	vk2d::Vector2u windowSize = window.window->GetSize() / 2;
	screenCordinates += Vector2d{ windowSize.x, windowSize.y };
	return screenCordinates.cast<int32_t>();
}

Matrix2i StarSystemLayer::toScreenCoordinates(Matrix2l gameCordinates){
	gameCordinates.row(0) -= viewOffset;
	gameCordinates.row(1) -= viewOffset;
	Matrix2d screenCordinates = gameCordinates.cast<double>();
	screenCordinates /= zoom * 1000; // m to km
	
	vk2d::Vector2u windowSize = window.window->GetSize() / 2;
	Matrix2d windowSizeMat;
	windowSizeMat << windowSize.x, windowSize.y, windowSize.x, windowSize.y;
	screenCordinates += windowSizeMat;
	return screenCordinates.cast<int32_t>();
}

