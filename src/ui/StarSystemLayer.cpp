/*
 * StarSystemLayer.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */
#include <GLFW/glfw3.h>

#include "Aurora.hpp"
#include "StarSystemLayer.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/components/Components.hpp"
#include "galaxy/Galaxy.hpp"
#include "utils/Math.hpp"
#include "utils/RenderUtils.hpp"

StarSystemLayer::StarSystemLayer(AuroraWindow& parentWindow, StarSystem* starSystem): UILayer(parentWindow) {
	this->starSystem = starSystem;
}

StarSystemLayer::~StarSystemLayer() {
}

void StarSystemLayer::drawEntities() {
	auto view = starSystem->registry.view<TimedMovementComponent, RenderComponent, CircleComponent>();
	
	for (entt::entity entity : view) {
		CircleComponent& circle = view.get<CircleComponent>(entity);
		
		if (!starSystem->registry.has<StrategicIconComponent>(entity) || !inStrategicView(entity, circle)) {

			TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
			MovementValues position = movement.get(Aurora.galaxy->time).value;
			Vector2l renderPosition = position.getPositionInKM() - viewOffset;

			TintComponent* tintComponent = starSystem->registry.try_get<TintComponent>(entity);
			vk2d::Colorf color = tintComponent != nullptr ? tintComponent->color : vk2d::Colorf::WHITE();
//			shapeRenderer.color = sRGBtoLinearRGB(Color(tintComponent?.color ?: Color.WHITE))

			float radius = std::max(1.0f, circle.radius / 1000);
			if (radius < 1) {
				window.window->DrawPoint(vectorToVK2D(renderPosition), color, 1);
			} else {
//				window.window->DrawEllipse(area, true, getCircleSegments(radius), color);
			}
		}
	}
}

void StarSystemLayer::render() {
	{	
		std::unique_lock<LockableBase(std::recursive_mutex)> lock(Aurora.galaxy->shadowLock);
		profilerEvents.clear();
		profilerEvents.start("render");
		
		profilerEvents.start("setup");
		
		
	//	val selectedEntityIDs = Player.current.selection.filter { it.system == starSystem && world.entityManager.isActive(it.entityID) && familyAspect.isInterested(it.entityID) }.map { it.entityID };
	
		int displaySize = hypot(window.window->GetSize().x, window.window->GetSize().y);
		
		profilerEvents.end();
		
	//		gravSystem.render(viewport, cameraOffset);
		
		//TODO dont interpolate new positions if timeDiff * velocity is not noticable at current zoom level 
		
		profilerEvents.start("drawDetections");
	//	drawDetections(entityIDs);
		profilerEvents.end();
	
		if (window.isKeyPressed(GLFW_KEY_C)) {
			profilerEvents.start("drawSelectionDetectionZones");
	//		drawSelectionDetectionZones(selectedEntityIDs);
			profilerEvents.end();
		}
		
		if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			profilerEvents.start("drawOrders");
	//		drawOrders();
			profilerEvents.end();
		}
		
		profilerEvents.start("renderOrbits");
	//	renderOrbits(cameraOffset);
		profilerEvents.end();
		
		profilerEvents.start("drawWeaponRanges");
	//	drawWeaponRanges(entityIDs, selectedEntityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawEntities");
		drawEntities();
		profilerEvents.end();
		
		profilerEvents.start("drawEntityCenters");
	//	drawEntityCenters(entityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawProjectiles");
	//	drawProjectiles();
		profilerEvents.end();
		
		profilerEvents.start("drawTimedMovement");
	//	drawTimedMovement(entityIDs, selectedEntityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawSelections");
	//	drawSelections(selectedEntityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawSelectionMoveTargets");
	//	drawSelectionMoveTargets(selectedEntityIDs);
		profilerEvents.end();
		
		//TODO draw selection weapon ranges
		profilerEvents.start("drawAttackTargets");
	//	drawAttackTargets(selectedEntityIDs);
		profilerEvents.end();
	
	//	spriteBatch.projectionMatrix = viewport.camera.combined;
		
		profilerEvents.start("drawStrategicEntities");
	//	drawStrategicEntities(entityIDs);
		profilerEvents.end();
		
		if (Aurora.settings.render.debugSpatialPartitioning) {
			profilerEvents.start("drawSpatialPartitioning");
	//		drawSpatialPartitioning();
			profilerEvents.end();
		}
		
		if (Aurora.settings.render.debugSpatialPartitioningPlanetoids) {
			profilerEvents.start("drawSpatialPartitioningPlanetoids");
	//		drawSpatialPartitioningPlanetoids();
			profilerEvents.end();
		}
		
		profilerEvents.start("spriteBatch");
	//	spriteBatch.projectionMatrix = uiCamera.combined;
	//	spriteBatch.begin();
		
		profilerEvents.start("drawSelectionDetectionStrength");
	//	drawSelectionDetectionStrength(selectedEntityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawNames");
	//	drawNames(entityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawMovementTimes");
	//	drawMovementTimes(entityIDs, selectedEntityIDs);
		profilerEvents.end();
	
	//	spriteBatch.end();
		profilerEvents.end();
		
		profilerEvents.end();
	}
	
//	if (dragSelect) {
//		shapeRenderer.projectionMatrix = uiCamera.combined
//		shapeRenderer.color = Color.WHITE
//		shapeRenderer.begin(ShapeRenderer.ShapeType.Line)
//
//		val dragSelection = getDragSelection(false)
//		shapeRenderer.rect(dragSelection.x.toFloat(), (dragSelection.y).toFloat(), dragSelection.width.toFloat(), dragSelection.height.toFloat())
//
//		shapeRenderer.end()
//	}
//	
//	val now = System.currentTimeMillis()
//		
//	if (now - lastTickrateUpdate > 1000) {
//		lastTickrateUpdate = now
//		galaxyTickrate = galaxy.time - oldGalaxyTime
//		oldGalaxyTime = galaxy.time
//	}
//	
//	val spriteBatch = AuroraGame.currentWindow.spriteBatch
//	val uiCamera = AuroraGame.currentWindow.screenService.uiCamera
//	
//	spriteBatch.projectionMatrix = uiCamera.combined
//	spriteBatch.begin()
//	
//	val y = 28f
//	var x = 4f
//	x += Assets.fontUI.draw(spriteBatch, "${Units.daysToDate(galaxy.day)} ${Units.secondsToString(galaxy.time)}, ", x, y).width
//	
//	if (galaxy.speed == 0L) {
//		Assets.fontUI.color = Color.RED
//		x += Assets.fontUI.draw(spriteBatch, "System Error", x, y).width
//		Assets.fontUI.color = Color.WHITE
//		
//	} else if (galaxy.speed < 0L) {
//		Assets.fontUI.color = Color.GRAY
//		x += Assets.fontUI.draw(spriteBatch, "speed ${Units.NANO_SECOND / -galaxy.speed}", x, y).width
//		Assets.fontUI.color = Color.WHITE
//		
//	} else if (galaxy.speedLimited) {
//		Assets.fontUI.color = Color.RED
//		x += Assets.fontUI.draw(spriteBatch, "speed ${Units.NANO_SECOND / galaxy.speed}", x, y).width
//		Assets.fontUI.color = Color.WHITE
//		
//	}  else {
//		x += Assets.fontUI.draw(spriteBatch, "speed ${Units.NANO_SECOND / galaxy.speed}", x, y).width
//	}
//	
//	x += Assets.fontUI.draw(spriteBatch, " ${galaxy.tickSize}", x, y).width
//	x += Assets.fontUI.draw(spriteBatch, " ${system.updateTimeAverage.toInt() / 1000}us ${galaxyTickrate}t/s", x, y).width
//	x += Assets.fontUI.draw(spriteBatch, ", ${allSubscription.getEntityCount()}st", x, y).width
//	
//	var str = "zoom $zoomLevel"
//	Assets.fontUI.cache.clear()
//	val strWidth = Assets.fontUI.cache.addText(str, 0f, 0f) .width
//	Assets.fontUI.cache.clear()
//	Assets.fontUI.draw(spriteBatch, str, Gdx.graphics.width - strWidth - 4f, y)
//	
//	spriteBatch.end()
}

bool StarSystemLayer::inStrategicView(entt::entity entity, CircleComponent& circle) {
	if (Aurora.settings.render.debugDisableStrategicView || zoom == 1.0f) {
		return false;
	}

	float radius = circle.radius / 1000;
	return radius / zoom < 5.0f;
}

int StarSystemLayer::getCircleSegments(float radius) {
	return std::min(1000, std::max(3, (int) (10 * std::cbrt(radius / zoom))));
}

bool StarSystemLayer::eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	return false;
}

bool StarSystemLayer::eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
	return false;
}

bool StarSystemLayer::eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	return false;
}

bool StarSystemLayer::eventScroll(vk2d::Vector2d scroll) {
	return false;
}
