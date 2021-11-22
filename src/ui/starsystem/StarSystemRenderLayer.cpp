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

#undef PROFILE
#undef PROFILE_End
#define PROFILE(x) if (window.profiling) profilerEvents.start((x));
#define PROFILE_End() if (window.profiling) profilerEvents.end();

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
		
		if (window.profiling) {
			profilerEvents.clear();
		}
		
		PROFILE("render");
		
		PROFILE("setup");
		
	//	val selectedEntityIDs = Player::current->selection.filter { it.starSystem == starSystem && world.entityManager.isActive(it.entityID) && familyAspect.isInterested(it.entityID) }.map { it.entityID };
	
		int displaySize = hypot(window.window->GetSize().x, window.window->GetSize().y);
		
		PROFILE_End();
		
	//		gravSystem.render(viewport, cameraOffset);
		
		//TODO dont interpolate new positions if timeDiff * velocity is not noticable at current zoom level
		
		PROFILE("drawDetections");
	//	drawDetections(entityIDs);
		PROFILE_End();
	
		if (window.isKeyPressed(GLFW_KEY_C)) {
			PROFILE("drawSelectionDetectionZones");
	//		drawSelectionDetectionZones(selectedEntityIDs);
			PROFILE_End();
		}
		
		if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			PROFILE("drawOrders");
	//		drawOrders();
			PROFILE_End();
		}
		
		PROFILE("renderOrbits");
	//	renderOrbits(cameraOffset);
		PROFILE_End();
		
		PROFILE("drawWeaponRanges");
	//	drawWeaponRanges(entityIDs, selectedEntityIDs);
		PROFILE_End();
		
		PROFILE("drawEntities");
		drawEntities();
		PROFILE_End();
		
		PROFILE("drawEntityCenters");
	//	drawEntityCenters(entityIDs);
		PROFILE_End();
		
		PROFILE("drawProjectiles");
	//	drawProjectiles();
		PROFILE_End();
		
		PROFILE("drawTimedMovement");
	//	drawTimedMovement(entityIDs, selectedEntityIDs);
		PROFILE_End();
		
		PROFILE("drawSelections");
	//	drawSelections(selectedEntityIDs);
		PROFILE_End();
		
		PROFILE("drawSelectionMoveTargets");
	//	drawSelectionMoveTargets(selectedEntityIDs);
		PROFILE_End();
		
		//TODO draw selection weapon ranges
		PROFILE("drawAttackTargets");
	//	drawAttackTargets(selectedEntityIDs);
		PROFILE_End();
	
	//	spriteBatch.projectionMatrix = viewport.camera.combined;
		
		PROFILE("drawStrategicEntities");
	//	drawStrategicEntities(entityIDs);
		PROFILE_End();
		
		PROFILE("drawSelectionDetectionStrength");
	//	drawSelectionDetectionStrength(selectedEntityIDs);
		PROFILE_End();
		
		PROFILE("drawNames");
	//	drawNames(entityIDs);
		PROFILE_End();
		
		PROFILE("drawMovementTimes");
	//	drawMovementTimes(entityIDs, selectedEntityIDs);
		PROFILE_End();
		
		PROFILE_End();
}

