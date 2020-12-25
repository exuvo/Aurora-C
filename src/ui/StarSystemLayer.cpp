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
#include "starsystems/StarSystemShadow.hpp"
#include "starsystems/components/Components.hpp"
#include "galaxy/Galaxy.hpp"
#include "utils/Math.hpp"
#include "utils/RenderUtils.hpp"
#include "ui/KeyMappings.hpp"

StarSystemLayer::StarSystemLayer(AuroraWindow& parentWindow, StarSystem* starSystem): UILayer(parentWindow) {
	this->starSystem = starSystem;
	
	zoomLevel = std::log(zoom / std::log(Aurora.settings.render.zoomSensitivity));
}

StarSystemLayer::~StarSystemLayer() {
}

void StarSystemLayer::drawEntities() {
	auto view = starSystem->registry.view<TimedMovementComponent, RenderComponent, CircleComponent>();
	
	for (entt::entity entity : view) {
		CircleComponent& circle = view.get<CircleComponent>(entity);
		
		if (!starSystem->registry.has<StrategicIconComponent>(entity) || !inStrategicView(entity, circle)) {

			TimedMovementComponent& movement = view.get<TimedMovementComponent>(entity);
			Vector2l position = movement.get(Aurora.galaxy->time).value.position;
			Vector2i renderPosition = toScreenCoordinates(position);

			TintComponent* tintComponent = starSystem->registry.try_get<TintComponent>(entity);
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

void StarSystemLayer::render() {
	
	int hDirection = 0;
	int vDirection = 0;

	if (window.isKeyPressed(GLFW_KEY_LEFT)) {
		hDirection--;
	}

	if (window.isKeyPressed(GLFW_KEY_RIGHT)) {
		hDirection++;
	}

	if (window.isKeyPressed(GLFW_KEY_DOWN)) {
		vDirection++;
	}

	if (window.isKeyPressed(GLFW_KEY_UP)) {
		vDirection--;
	}

	if (hDirection != 0 || vDirection != 0) {
		viewOffset += (Vector2f{ hDirection, vDirection } * 1000 * zoom).cast<int64_t>();
	}
	
	if (commandMenuPotentialStart && (getMillis() - commandMenuPotentialStartTime > 200ms || vectorDistance(commandMenuPotentialStartPos, getMouseInScreenCordinates()) > 50)) {
		commandMenuPotentialStart = false;
		
//		uiScreen.openCommandMenu();
	}
	
	if (dragPotentialStart && vectorDistance(commandMenuPotentialStartPos, getMouseInScreenCordinates()) > 50) {
		dragPotentialStart = false;
		
		

		
	}
	
//	if (dragSelectionPotentialStart) {
//
//			val dx = dragX - screenX;
//			val dy = dragY - screenY;
//
//			if (std::sqrt((dx * dx + dy * dy).toDouble()) > 10) {
//
//				dragSelectionPotentialStart = false;
//				dragSelecting = true;
////				println("drag select start")
//			}
//		}
//	
//	if (movingWindow) {
//			var mouseScreenNow = Vector3(screenX.toFloat(), screenY.toFloat(), 0f)
//			var mouseWorldNow = camera.unproject(mouseScreenNow.cpy())
//			var mouseWorldBefore = camera.unproject(mouseScreenNow.cpy().add(Vector3((dragX - screenX).toFloat(), (dragY - screenY).toFloat(), 0f)))
//
//			var diff = mouseWorldNow.cpy().sub(mouseWorldBefore)
//
//			cameraOffset.sub(diff.x.toLong(), diff.y.toLong())
//
//			//TODO ensure camera position is always inside the solar system
//
//			dragX = screenX;
//			dragY = screenY;
//
//			return true;
//		}
	
	{
		std::unique_lock<LockableBase(std::recursive_mutex)> lock(Aurora.galaxy->shadowLock);
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
		
		if (dragSelecting) {
			profilerEvents.start("drawSelecting");
			window.window->DrawRectangle({}, false, vk2d::Colorf::WHITE());
			
			
			profilerEvents.end();
		}
		
		profilerEvents.end();
	}
	
	if (dragSelecting) {
		Matrix2i selection = getDragSelection();
		window.window->DrawRectangle(matrixToVK2D(selection), false, vk2d::Colorf::WHITE());
	}
	
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
//	x += Assets.fontUI.draw(spriteBatch, " ${starSystem.updateTimeAverage.toInt() / 1000}us ${galaxyTickrate}t/s", x, y).width
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

bool StarSystemLayer::keyAction(KeyActions_StarSystemLayer action) {
	
	if (action == KeyActions_StarSystemLayer::GENERATE_SYSTEM) {

		printf("GENERATE_SYSTEM\n"); fflush(stdout);
//			StarSystemGeneration(starSystem).generateRandomSystem();

	} else if (action == KeyActions_StarSystemLayer::SPEED_UP) {
		
		Player::current->increaseSpeed();
		return true;

	} else if (action == KeyActions_StarSystemLayer::SPEED_DOWN) {

		Player::current->decreaseSpeed();
		return true;

	} else if (action == KeyActions_StarSystemLayer::PAUSE) {

		Player::current->pauseSpeed();
		return true;

	} else if (action == KeyActions_StarSystemLayer::MAP) {

		printf("MAP\n"); fflush(stdout);
//			window.setMainLayer(GalaxyLayer(window, starSystem));

	} else if (action == KeyActions_StarSystemLayer::ATTACK) {

//			if (Player::current->selection.isNotEmpty()) {
//				selectedAction = KeyActions_StarSystemLayer::ATTACK;
//				println("Selected action " + action);
//			} else {
//				println("Unable to select action " + action + ", no selection");
//			}
	}

	return false;
}

bool StarSystemLayer::eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	KeyActions_StarSystemLayer keyBind = KeyMappings::getRaw<KeyActions_StarSystemLayer>(scancode, action, modifier_keys);
	
	if (keyBind != KeyActions_StarSystemLayer::NONE) {
		return keyAction(keyBind);
	}

	return false;
}

bool StarSystemLayer::eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
	printf("character %c\n", character); fflush(stdout);
	KeyActions_StarSystemLayer keyBind = KeyMappings::getTranslated<KeyActions_StarSystemLayer>(character);

	if (keyBind != KeyActions_StarSystemLayer::NONE) {
		return keyAction(keyBind);
	}

	return false;
}

bool StarSystemLayer::eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	
	if (action == vk2d::ButtonAction::PRESS) {
		
		if (!movingWindow && !dragSelecting) {
			
			if (button == vk2d::MouseButton::BUTTON_LEFT) {
				commandMenuPotentialStart = false;
				
				{
					std::unique_lock<LockableBase(std::recursive_mutex)> lock(Aurora.galaxy->shadowLock);
					
//					val directSelectionSubscription = system.shadow.world.getAspectSubscriptionManager().get(DIRECT_SELECTION_FAMILY)
//					val weaponFamilyAspect = system.shadow.world.getAspectSubscriptionManager().get(WEAPON_FAMILY).aspect
//					val renderSystem = system.shadow.world.getSystem(RenderSystem::class.java)
//					
//					val shadow = system.shadow
//					val mouseInGameCoordinates = toWorldCordinates(getMouseInScreenCordinates(screenX, screenY))
//					val entitiesUnderMouse = Bag<EntityReference>()
//					val testCircle = CircleL()
//					val zoom = camera.zoom
////						val entityIDs = directSelectionSubscription.entities
//					val entityIDs = SpatialPartitioningSystem.query(system.shadow.quadtreeShips,
//							mouseInGameCoordinates.x - 1, mouseInGameCoordinates.y - 1,
//							mouseInGameCoordinates.x + 1, mouseInGameCoordinates.y + 1)
//					
//					entityIDs.addAll(SpatialPartitioningPlanetoidsSystem.query(system.shadow.quadtreePlanetoids,
//							mouseInGameCoordinates.x - 1, mouseInGameCoordinates.y - 1,
//							mouseInGameCoordinates.x + 1, mouseInGameCoordinates.y + 1))
//					
////						println("entityIDs $entityIDs")
//					
//					// Exact check first
//					entityIDs.forEachFast { entityID ->
//						if (directSelectionSubscription.aspect.isInterested(entityID)) {
//							val position = shadow.movementMapper.get(entityID).get(galaxy.time).value.position
//							val radius: Float
//
//							if (renderSystem.inStrategicView(entityID, zoom)) {
//
//								radius = 1000 * zoom * RenderSystem.STRATEGIC_ICON_SIZE / 2
//
//							} else {
//
//								radius = shadow.circleMapper.get(entityID).radius
//							}
//
//							testCircle.set(position, radius)
//
//							if (testCircle.contains(mouseInGameCoordinates)) {
//								entitiesUnderMouse.add(system.getEntityReference(entityID))
//							}
//						}
//					}
//
//					// Lenient check if empty
//					if (entitiesUnderMouse.isEmpty()) {
//						entityIDs.forEachFast { entityID ->
//							if (directSelectionSubscription.aspect.isInterested(entityID)) {
//								val position = shadow.movementMapper.get(entityID).get(galaxy.time).value.position
//								var radius: Float = 1000 * 2 * camera.zoom
//
//								if (renderSystem.inStrategicView(entityID, zoom)) {
//
//									radius += 1000 * zoom * RenderSystem.STRATEGIC_ICON_SIZE / 2
//
//								} else {
//
//									radius += shadow.circleMapper.get(entityID).radius
//								}
//								testCircle.set(position, radius)
//
//								if (testCircle.contains(mouseInGameCoordinates)) {
//									entitiesUnderMouse.add(system.getEntityReference(entityID))
//								}
//							}
//						}
//
//						if (entitiesUnderMouse.isNotEmpty()) {
////								println("lenient selected ${entitiesUnderMouse.size()} entities")
//						}
//
//					} else {
////							println("strict selected ${entitiesUnderMouse.size()} entities")
//					}
//
//					if (selectedAction == null) {
//
//						if (entitiesUnderMouse.isNotEmpty()) {
//
//							dragSelectPotentialStart = false;
//
//							if (Player.current.selection.isNotEmpty() && !Gdx.input.isKeyPressed(Input.Keys.SHIFT_LEFT)) {
//								Player.current.selection.clear()
////								println("cleared selection")
//							}
//
//							Player.current.selection.addAll(entitiesUnderMouse)
//							
//						} else {
//
//							dragSelectPotentialStart = true;
//							dragX = screenX;
//							dragY = screenY;
//
////						println("drag select potential dragX $dragX, dragY $dragY")
//						}
//
//					} else if (selectedAction == KeyActions_StarSystemScreen.ATTACK) {
//						selectedAction = null
//
//						if (Player.current.selection.isNotEmpty()) {
//							val selectedEntities = Player.current.selection.filter { entityRef ->
//								system == entityRef.system && weaponFamilyAspect.isInterested(entityRef.entityID)
//							}
//							
//							if (selectedEntities.isEmpty()) {
//								println("No combat capable ship selected")
//								
//							} else {
//
//								var targetRef: EntityReference? = null
//								
//								if (entitiesUnderMouse.isNotEmpty()) {
//									targetRef = entitiesUnderMouse[0]
//									println("Attacking ${printEntity(targetRef.entityID, targetRef.system.world)}")
//									
//								} else {
//									println("Clearing attack target")
//								}
//								
//								selectedEntities.forEachFast{ entityRef ->
//									val activeTCs = shadow.activeTargetingComputersComponentMapper.get(entityRef.entityID)
//									
//									activeTCs?.targetingComputers?.forEachFast{ tc ->
//										Player.current.empire!!.commandQueue.add(EntityClearTargetCommand(entityRef, tc))
//										
//										if (targetRef != null) {
//											Player.current.empire!!.commandQueue.add(EntitySetTargetCommand(entityRef, tc, targetRef))
//										}
//									}
//									
//									if (targetRef != null) {
//										val idleTCs = shadow.idleTargetingComputersComponentMapper.get(entityRef.entityID)
//										
//										idleTCs?.targetingComputers?.forEachFast{ tc ->
//											Player.current.empire!!.commandQueue.add(EntitySetTargetCommand(entityRef, tc, targetRef))
//										}
//									}
//								}
//							}
//						}
//					}

					return true;
				}
				
			} else if (button == vk2d::MouseButton::BUTTON_MIDDLE) {
			
				selectedAction = nullptr;
				commandMenuPotentialStart = false;
				movingWindow = true;
				dragStart = vk2dToVector(window.window->GetCursorPosition()).cast<int32_t>();
				return true;
				
			} else if (button == vk2d::MouseButton::BUTTON_RIGHT) {
				
				selectedAction = nullptr;
				dragSelectionPotentialStart = false;

				commandMenuPotentialStart = true;
				commandMenuPotentialStartTime = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
				commandMenuPotentialStartPos = vk2dToVector(window.window->GetCursorPosition()).cast<int32_t>();
			}
			
		} else {
			
			if (dragSelecting && button != vk2d::MouseButton::BUTTON_LEFT) {
				dragSelecting = false;
				return true;
			}

			if (movingWindow && button != vk2d::MouseButton::BUTTON_MIDDLE) {
				movingWindow = false;
				//TODO
//				viewOffset = 
				return true;
			}
		}
		
	} else if (action == vk2d::ButtonAction::RELEASE) {
		
		if (movingWindow && button == vk2d::MouseButton::BUTTON_MIDDLE) {
			movingWindow = false;
			return true;
		}
		
		if (button == vk2d::MouseButton::BUTTON_LEFT) {
			if (dragSelecting) {
				if (Player::current->selection.size() > 0 && window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
					Player::current->selection.clear();
					printf("cleared selection\n"); fflush(stdout);
				}
	
				Matrix2i dragSelection = getDragSelection();
				std::cout << "dragSelection " << dragSelection << std::endl;
	
				Matrix2l worldCoordinates = toWorldCoordinates(dragSelection);
				std::cout << "worldCoordinates " << worldCoordinates << std::endl;
	
				std::vector<EntityReference> entitiesInSelection {};
				
				{
					std::unique_lock<LockableBase(std::recursive_mutex)> lock(Aurora.galaxy->shadowLock);
					
					SmallList<entt::entity> entities = SpatialPartitioningSystem::query(starSystem->shadow->quadtreeShips, worldCoordinates);
//					println("worldCoordinates $worldCoordinates, entities $entities")
					
					for (entt::entity entity : entities) {
						if (starSystem->shadow->registry.has<TimedMovementComponent, RenderComponent>(entity)) {
							TimedMovementComponent& movement = starSystem->shadow->registry.get<TimedMovementComponent>(entity);
							Vector2l position = movement.get(Aurora.galaxy->time).value.position;
			
							if (rectagleContains(worldCoordinates, position)) {
								entitiesInSelection.push_back(starSystem->shadow->getEntityReference(entity));
							}
						}
					}
				}
	
				if (entitiesInSelection.size() > 0) {
					vectorAppend(Player::current->selection, entitiesInSelection);
	//				println("drag selected ${entitiesInSelection.size} entities")
				}
	
				dragSelecting = false;
				return true;
			}
	
			if (dragSelectionPotentialStart) {
				if (Player::current->selection.size() > 0 && !window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
					Player::current->selection.clear();
	//				println("cleared selection")
				}
			}
		}
		
		if (button == vk2d::MouseButton::BUTTON_RIGHT && commandMenuPotentialStart) {
			
			commandMenuPotentialStart = false;
			
			if (Player::current->selection.size() > 0) {

				std::unique_lock<LockableBase(std::recursive_mutex)> lock(Aurora.galaxy->shadowLock);
				
//				val movementFamilyAspect = starSystem.shadow.world.getAspectSubscriptionManager().get(MovementSystem.CAN_ACCELERATE_FAMILY).aspect
//				val directSelectionSubscription = starSystem.shadow.world.getAspectSubscriptionManager().get(DIRECT_SELECTION_FAMILY)
//				val renderSystem = starSystem.shadow.world.getSystem(RenderSystem::class.java)
//				
//				val selectedEntities = Player::current->selection.filter { entityRef ->
//					starSystem == entityRef.starSystem && movementFamilyAspect.isInterested(entityRef.entityID)
//				}
//
//				if (selectedEntities.isNotEmpty()) {
//					
//					val shadow = starSystem.shadow
//					val mouseInGameCoordinates = toWorldCordinates(getMouseInScreenCordinates(screenX, screenY))
//					val entitiesUnderMouse = IntBag()
//					val entityIDs = directSelectionSubscription.entities
//					val testCircle = CircleL()
//					val zoom = camera.zoom
//
//					entityIDs.forEachFast { entityID ->
//						val position = shadow.movementMapper.get(entityID).get(galaxy.time).value.position
//						val radius: Float
//
//						if (renderSystem.inStrategicView(entityID, zoom)) {
//
//							radius = 1000 * zoom * RenderSystem.STRATEGIC_ICON_SIZE / 2
//
//						} else {
//
//							radius = shadow.circleMapper.get(entityID).radius
//						}
//
//						testCircle.set(position, radius)
//
//						if (testCircle.contains(mouseInGameCoordinates)) {
//							entitiesUnderMouse.add(entityID)
//						}
//					}
//
//					if (!entitiesUnderMouse.isEmpty) {
//
////							println("Issuing move to entity order")
//
//						val targetEntityID = entitiesUnderMouse.get(0)
//						var approachType = ApproachType.BRACHISTOCHRONE
//
//						if (Gdx.input.isKeyPressed(Input.Keys.CONTROL_LEFT)) {
//							approachType = ApproachType.BALLISTIC
//						}
//
////								println("Queuing move to entity command for $entityRef to $targetEntityID")
//						Player::current->empire!!.commandQueue.add(EntitiesMoveToEntityCommand(Bag(selectedEntities), shadow.getEntityReference(targetEntityID), approachType))
//
//					} else {
//
////							println("Issuing move to position order")
//
//						val targetPosition = mouseInGameCoordinates
//						var approachType = ApproachType.BRACHISTOCHRONE
//
//						if (Gdx.input.isKeyPressed(Input.Keys.CONTROL_LEFT)) {
//							approachType = ApproachType.BALLISTIC
//						}
//
////								println("Queuing move to position command for $entityRef to $targetPosition")
//						Player::current->empire.commandQueue.add(EntitiesMoveToPositionCommand(Bag(selectedEntities), targetPosition, approachType))
//					}
//
//					return true;
//				}
			}
		}
	}
	
	return false;
}

bool StarSystemLayer::eventScroll(vk2d::Vector2d scroll) {
	zoomLevel -= scroll.y;
	if (zoomLevel < 0) {
		zoomLevel = 0;
	}

	// camera.zoom >= 1
	float newZoom = std::pow(Aurora.settings.render.zoomSensitivity, (double) zoomLevel);

//	std::cout << "zoom:" << newZoom << ", zoomLevel:" << zoomLevel << std::endl;

	if (newZoom > maxZoom) {
		newZoom = maxZoom;
		zoomLevel = (std::log(newZoom) / std::log(Aurora.settings.render.zoomSensitivity));
	}

	if (scroll.y > 0) {
		// Det som var under musen innan scroll ska fortsätta vara där efter zoom
		// http://stackoverflow.com/questions/932141/zooming-an-object-based-on-mouse-position

		Vector2l diff = viewOffset - toWorldCoordinates(getMouseInScreenCordinates());
		diff -= (diff.cast<double>() * (1 / zoom) * newZoom).cast<int64_t>();
		viewOffset -= diff;

		//TODO ensure viewOffset is always inside the solar system
	}
	
	zoom = newZoom;

	//TODO allow zooming out to galaxy level

	return true;
}

Matrix2i StarSystemLayer::getDragSelection() {
	vk2d::Vector2d cursor = window.window->GetCursorPosition();
	
	Matrix2i mat {};
	
	if (cursor.x >= dragStart.x()) {
		mat(0,0) = cursor.x;
		mat(1,0) = dragStart.x();
	} else {
		mat(0,0) = dragStart.x();
		mat(1,0) = cursor.x;
	}
	
	if (cursor.y >= dragStart.y()) {
		mat(0,1) = cursor.y;
		mat(1,1) = dragStart.y();
	} else {
		mat(0,1) = dragStart.y();
		mat(1,1) = cursor.y;
	}
	
	return mat;
}

bool StarSystemLayer::inStrategicView(entt::entity entity, CircleComponent& circle) {
	if (Aurora.settings.render.debugDisableStrategicView || zoom == 1.0f) {
		return false;
	}

	float radius = circle.radius / 1000;
	return radius / zoom < 5.0f;
}

// radius in screen space radius
int StarSystemLayer::getCircleSegments(float radius) {
	return std::min(1000, std::max(3, (int) (9 * std::cbrt(radius))));
}

Vector2i StarSystemLayer::getMouseInScreenCordinates() {
	vk2d::Vector2d cursor = window.window->GetCursorPosition();
	vk2d::Vector2u windowSize = window.window->GetSize();
	windowSize /= 2;
	cursor -= vk2d::Vector2d{ windowSize.x, windowSize.y };
	return { cursor.x, cursor.y };
}

Vector2l StarSystemLayer::toWorldCoordinates(Vector2i screenCoordinates) {
	Vector2d worldCoordinates = screenCoordinates.cast<double>();
	worldCoordinates *= 1000 * zoom; // km to m
	return worldCoordinates.cast<int64_t>() + viewOffset;
}

Matrix2l StarSystemLayer::toWorldCoordinates(Matrix2i screenCoordinates) {
	Matrix2d worldCoordinates = screenCoordinates.cast<double>();
	worldCoordinates *= 1000 * zoom; // km to m
	Matrix2l worldCoordinates2 = worldCoordinates.cast<int64_t>();
	worldCoordinates2.row(0) += viewOffset;
	worldCoordinates2.row(1) += viewOffset;
	return worldCoordinates2;
}

Vector2i StarSystemLayer::toScreenCoordinates(Vector2l gameCordinates){
	gameCordinates -= viewOffset;
	Vector2d gameCordinates2 = gameCordinates.cast<double>();
	gameCordinates2 /= zoom * 1000; // m to km
	return gameCordinates2.cast<int32_t>();
}

Matrix2i StarSystemLayer::toScreenCoordinates(Matrix2l gameCordinates){
	gameCordinates.row(0) -= viewOffset;
	gameCordinates.row(1) -= viewOffset;
	Matrix2d gameCordinates2 = gameCordinates.cast<double>();
	gameCordinates2 /= zoom * 1000; // m to km
	return gameCordinates2.cast<int32_t>();
}

