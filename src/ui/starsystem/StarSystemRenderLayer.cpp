/*
 * StarSystemRenderLayer.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */
#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include "Aurora.hpp"
#include "StarSystemRenderLayer.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/ShadowStarSystem.hpp"
#include "starsystems/components/Components.hpp"
#include "galaxy/Galaxy.hpp"
#include "ui/KeyMappings.hpp"

StarSystemRenderLayer::StarSystemRenderLayer(AuroraWindow& parentWindow): StarSystemLayer(parentWindow) {
}

StarSystemRenderLayer::~StarSystemRenderLayer() {
}

void StarSystemRenderLayer::drawEntities() {
	
	entt::registry* registry;
	
	if (Aurora.settings.render.useShadow) {
		registry = &starSystem->shadow->registry;
	} else {
		registry = &starSystem->registry;
	}
	
	auto view = registry->view<TimedMovementComponent, RenderComponent, CircleComponent>();
	
	for (entt::entity entity : view) {
		CircleComponent& circle = view.get<CircleComponent>(entity);
		
		if (!registry->all_of<StrategicIconComponent>(entity) || !inStrategicView(entity, circle)) {

			TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
			Vector2l position = movement.get(Aurora.galaxy->time).value.position;
			Vector2i renderPosition = toScreenCoordinates(position);

			TintComponent* tintComponent = registry->try_get<TintComponent>(entity);
			vk2d::Colorf color = tintComponent != nullptr ? tintComponent->color : vk2d::Colorf::WHITE();
//			shapeRenderer.color = sRGBtoLinearRGB(Color(tintComponent?.color ?: Color.WHITE))

			
			float radius = std::max(1.0f, circle.radius / 1000) / zoom;
//			std::cout << "draw entity: pos " << position << ", rpos " << renderPosition << ", radius " << radius << std::endl; 
			if (radius <= 0.5) {
				window.window->DrawPoint(vectorToVK2D(renderPosition), color, 1);
			} else {
				window.window->DrawEllipse(vk2d::Rect2f {renderPosition.x() - radius, renderPosition.y() - radius, renderPosition.x() + radius, renderPosition.y() + radius}, true, getCircleSegments(radius), color);
			}
		}
	}
}

void StarSystemRenderLayer::render() {
		std::unique_lock<LockableBase(std::mutex)> lock(Aurora.galaxy->shadowLock);
		
		profilerEvents.clear();
		profilerEvents.start("render");
		
		profilerEvents.start("setup");
		
	//	val selectedEntityIDs = Player::current->selection.filter { it.starSystem == starSystem && world.entityManager.isActive(it.entityID) && familyAspect.isInterested(it.entityID) }.map { it.entityID };
	
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
		
		profilerEvents.start("drawSelectionDetectionStrength");
	//	drawSelectionDetectionStrength(selectedEntityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawNames");
	//	drawNames(entityIDs);
		profilerEvents.end();
		
		profilerEvents.start("drawMovementTimes");
	//	drawMovementTimes(entityIDs, selectedEntityIDs);
		profilerEvents.end();
		
		profilerEvents.end();
}

