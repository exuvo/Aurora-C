/*
 * StarSystemInputLayer.cpp
 *
 *  Created on: Dec 17, 2020
 *      Author: exuvo
 */
#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include "Aurora.hpp"
#include "StarSystemInputLayer.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/ShadowStarSystem.hpp"
#include "starsystems/components/Components.hpp"
#include "galaxy/Galaxy.hpp"
#include "ui/KeyMappings.hpp"
#include "utils/Format.hpp"

StarSystemInputLayer::StarSystemInputLayer(AuroraWindow& parentWindow): StarSystemLayer(parentWindow) {
	zoomLevel = std::log(zoom) / std::log(Aurora.settings.render.zoomSensitivity);
}

StarSystemInputLayer::~StarSystemInputLayer() {
}

void StarSystemInputLayer::render() {
	
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
	
	if (tracking) {
		
		Vector2l centerOfSelection = { 0,0 };
		
		{
			std::unique_lock<LockableBase(std::mutex)> lock(Aurora.galaxy->shadowLock);
			
			for (auto it = Player::current->selection.begin(); it != Player::current->selection.end();) {
				const EntityReference* ref = &*it++;
				
				if (!ref->isValid(*starSystem->shadow)) {
					EntityReference cpy = *ref;
					Player::current->selectionSet.erase(*ref);
					
					if (!cpy.resolveReference(*starSystem->shadow)) {
						vectorEraseUnorderedIter(Player::current->selection, it);
						std::cout << "Removed invalid entity reference " << cpy << std::endl;
						it--;
						continue;
					}
					
					*it = cpy;
					ref = &*Player::current->selectionSet.insert(cpy).first;
				}
				
				if (ref->system == starSystem) {
					TimedMovementComponent movement = starSystem->shadow->registry.get<TimedMovementComponent>(ref->entityID);
					Vector2l position = movement.get(Aurora.galaxy->time).value.position;
					centerOfSelection += position;
				}
			}
		}
		
		if (Player::current->selection.size() > 0) {
			centerOfSelection /= Player::current->selection.size();
			viewOffset = centerOfSelection;
		} else {
			tracking = false;
		}
	}
	
	if (commandMenuPotentialStart && (getMillis() - commandMenuPotentialStartTime > 200ms || vectorDistance(dragStart, getMouseInScreenCordinates()) > 50)) {
		commandMenuPotentialStart = false;
		
		std::cout << "open command menu" << std::endl;
//		uiScreen.openCommandMenu();
	}
	
	if (dragSelectionPotentialStart && vectorDistance(dragStart, getMouseInScreenCordinates()) > 10) {
			dragSelectionPotentialStart = false;
			dragSelecting = true;
//			std::cout << "drag select start " << dragStart << " - " << getMouseInScreenCordinates() << " = " << vectorDistance(dragStart, getMouseInScreenCordinates()) << " ~ " << vectorDistanceFast(dragStart, getMouseInScreenCordinates()) << std::endl;
	}
	
	if (movingView) {
		Vector2i mouseScreenNow = getMouseInScreenCordinates();
		Vector2l mouseWorldNow = toWorldCoordinates(mouseScreenNow);
		Vector2l mouseWorldBefore = toWorldCoordinates(dragStart);

		Vector2l diff = mouseWorldNow - mouseWorldBefore;

		viewOffset -= diff;

		//TODO ensure camera position is always inside the solar system

		if (!tracking) {
			dragStart = mouseScreenNow;
		}
	}
}

bool StarSystemInputLayer::keyAction(KeyActions_StarSystemLayer action) {
	
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
	} else if (action == KeyActions_StarSystemLayer::TRACK) {
		
		if (tracking) {
			tracking = false;
			
			if (movingView) {
				dragStart = getMouseInScreenCordinates();
			}
			
		} else if (Player::current->selection.size() > 0) {
			tracking = true;
//			printf("tracking selection\n"); fflush(stdout);
		}
		
	}
	
	return false;
}

bool StarSystemInputLayer::eventKeyboard(vk2d::KeyboardButton button, int32_t scancode, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	KeyActions_StarSystemLayer keyBind = KeyMappings::getRaw<KeyActions_StarSystemLayer>(scancode, action, modifier_keys);
	
	if (keyBind != KeyActions_StarSystemLayer::NONE) {
		return keyAction(keyBind);
	}
	
	return false;
}

bool StarSystemInputLayer::eventCharacter(uint32_t character, vk2d::ModifierKeyFlags modifier_keys) {
//	printf("character %c\n", character); fflush(stdout);
	KeyActions_StarSystemLayer keyBind = KeyMappings::getTranslated<KeyActions_StarSystemLayer>(character);
	
	if (keyBind != KeyActions_StarSystemLayer::NONE) {
		return keyAction(keyBind);
	}
	
	return false;
}

bool StarSystemInputLayer::eventMouseButton(vk2d::MouseButton button, vk2d::ButtonAction action, vk2d::ModifierKeyFlags modifier_keys) {
	
	if (action == vk2d::ButtonAction::PRESS) {
		
		if (!movingView && !dragSelecting) {
			
			if (button == vk2d::MouseButton::BUTTON_LEFT) {
				commandMenuPotentialStart = false;
				
				{
					std::unique_lock<LockableBase(std::mutex)> lock(Aurora.galaxy->shadowLock);
					
//					val directSelectionSubscription = system.shadow.world.getAspectSubscriptionManager().get(DIRECT_SELECTION_FAMILY)
//					val weaponFamilyAspect = system.shadow.world.getAspectSubscriptionManager().get(WEAPON_FAMILY).aspect
//					val renderSystem = system.shadow.world.getSystem(RenderSystem::class.java)
					
					Vector2l mouseInGameCoordinates = toWorldCoordinates(getMouseInScreenCordinates());
//					std::cout << "mouseInGameCoordinates " << mouseInGameCoordinates << " getMouseInScreenCordinates " << getMouseInScreenCordinates() << std::endl;
					Matrix2l queryMatrix;
					queryMatrix << mouseInGameCoordinates.x() - 1, mouseInGameCoordinates.y() - 1, mouseInGameCoordinates.x() + 1, mouseInGameCoordinates.y() + 1;
	
					std::vector<EntityReference> entitiesUnderMouse {};
//						val entityIDs = directSelectionSubscription.entities
					SmallList<entt::entity> entities = SpatialPartitioningSystem::query(starSystem->shadow->quadtreeShips, queryMatrix);
					SmallList<entt::entity> entitiesPlanetoids = SpatialPartitioningPlanetoidsSystem::query(starSystem->shadow->quadtreePlanetoids, queryMatrix);
					
//					std::cout << "entities " << entities << " entitiesPlanetoids " << entitiesPlanetoids << std::endl;
					entities.append(entitiesPlanetoids);
					
					// Exact check first
					for (entt::entity entity : entities) {
						if (starSystem->shadow->registry.all_of<TimedMovementComponent, RenderComponent, CircleComponent>(entity)) {
							TimedMovementComponent& movementComponent = starSystem->shadow->registry.get<TimedMovementComponent>(entity);
							CircleComponent& circleComponent = starSystem->shadow->registry.get<CircleComponent>(entity);
							Vector2l position = movementComponent.get(Aurora.galaxy->time).value.position;
							float radiusInM;

							if (inStrategicView(entity, circleComponent)) {

								radiusInM = 1000 * zoom * STRATEGIC_ICON_SIZE / 2;

							} else {

								radiusInM = circleComponent.radius;
							}
							
//							std::cout << "dist " << vectorDistance(position, mouseInGameCoordinates) << " <= " << radiusInM << " pos " << position << std::endl;
							if (vectorDistance(position, mouseInGameCoordinates) <= radiusInM) {
								entitiesUnderMouse.push_back(starSystem->shadow->getEntityReference(entity));
							}
						}
					}

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
					if (selectedAction == nullptr) {

						if (entitiesUnderMouse.size() > 0) {

							dragSelectionPotentialStart = false;

							if (Player::current->selection.size() > 0 && !window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
								Player::current->clearSelection();
								printf("cleared selection\n");
							}
							
							std::cout << "added " << entitiesUnderMouse << " to selection" << std::endl;
							Player::current->addSelection(entitiesUnderMouse);
							
						} else {
//
							dragSelectionPotentialStart = true;
							dragStart = getMouseInScreenCordinates();

//							println("drag select potential dragX $dragX, dragY $dragY")
						}
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
					}
				}
				
				return true;
				
			} else if (button == vk2d::MouseButton::BUTTON_MIDDLE) {
			
				selectedAction = nullptr;
				commandMenuPotentialStart = false;
				movingView = true;
				dragStart = getMouseInScreenCordinates();
				return true;
				
			} else if (button == vk2d::MouseButton::BUTTON_RIGHT) {
				
				selectedAction = nullptr;
				dragSelectionPotentialStart = false;
				
				commandMenuPotentialStart = true;
				commandMenuPotentialStartTime = duration_cast<nanoseconds>(steady_clock::now().time_since_epoch());
				dragStart = getMouseInScreenCordinates();
			}
			
		} else {
			
			if (dragSelecting && button != vk2d::MouseButton::BUTTON_LEFT) {
				dragSelecting = false;
				return true;
			}
			
			if (movingView && button != vk2d::MouseButton::BUTTON_MIDDLE) {
				movingView = false;
				return true;
			}
		}
		
	} else if (action == vk2d::ButtonAction::RELEASE) {
		
		if (movingView && button == vk2d::MouseButton::BUTTON_MIDDLE) {
			movingView = false;
			return true;
		}
		
		if (button == vk2d::MouseButton::BUTTON_LEFT) {
			if (dragSelecting) {
				if (Player::current->selection.size() > 0 && !window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
					Player::current->clearSelection();
//					printf("cleared selection\n"); fflush(stdout);
				}
				
				Matrix2i dragSelection = getDragSelection();
//				std::cout << "dragSelection " << dragSelection << std::endl;
				
				Matrix2l worldCoordinates = toWorldCoordinates(dragSelection);
//				std::cout << "worldCoordinates " << worldCoordinates << std::endl;
				
				std::vector<EntityReference> entitiesInSelection {};
				
				{
					std::unique_lock<LockableBase(std::mutex)> lock(Aurora.galaxy->shadowLock);
					
					SmallList<entt::entity> entities = SpatialPartitioningSystem::query(starSystem->shadow->quadtreeShips, worldCoordinates);
//					std::cout << "worldCoordinates " << worldCoordinates << ", entities " << entities << std::endl;
					
					for (entt::entity entity : entities) {
						if (starSystem->shadow->registry.all_of<TimedMovementComponent, RenderComponent>(entity)) {
							TimedMovementComponent& movement = starSystem->shadow->registry.get<TimedMovementComponent>(entity);
							Vector2l position = movement.get(Aurora.galaxy->time).value.position;
			
							if (rectangleContains(worldCoordinates, position)) {
								entitiesInSelection.push_back(starSystem->shadow->getEntityReference(entity)); //TODO avoid duplicates
							}
						}
					}
				}
				
				if (entitiesInSelection.size() > 0) {
					Player::current->addSelection(entitiesInSelection);
					std::cout << "drag selected " << Player::current->selection.size() << " entities" << std::endl;
				}
				
				dragSelecting = false;
				return true;
			}
			
			if (dragSelectionPotentialStart) {
				dragSelectionPotentialStart = false;
				if (Player::current->selection.size() > 0 && !window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
					Player::current->clearSelection();
	//				println("cleared selection")
				}
			}
		}
		
		if (button == vk2d::MouseButton::BUTTON_RIGHT && commandMenuPotentialStart) {
			
			commandMenuPotentialStart = false;
			
			if (Player::current->selection.size() > 0) {
				
				std::unique_lock<LockableBase(std::mutex)> lock(Aurora.galaxy->shadowLock);
				
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

bool StarSystemInputLayer::eventScroll(vk2d::Vector2d scroll) {
	zoomLevel -= scroll.y;
	if (zoomLevel < 0) {
		zoomLevel = 0;
	}

	// camera.zoom >= 1
	float newZoom = std::pow(Aurora.settings.render.zoomSensitivity, (float) zoomLevel);

	if (newZoom > maxZoom) {
		newZoom = maxZoom;
		zoomLevel = std::log(newZoom) / std::log(Aurora.settings.render.zoomSensitivity);
	}
	
//	std::cout << "zoom:" << newZoom << ", zoomLevel:" << zoomLevel << std::endl;

	if (scroll.y > 0) {
		// Det som var under pekaren innan scroll ska fortsätta vara där efter zoom
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

Matrix2i StarSystemInputLayer::getDragSelection() {
	Vector2i cursor = getMouseInScreenCordinates();
	
	Matrix2i mat {};
	
	if (cursor.x() >= dragStart.x()) {
		mat(0,0) = dragStart.x();
		mat(1,0) = cursor.x();
	} else {
		mat(0,0) = cursor.x();
		mat(1,0) = dragStart.x();
	}
	
	if (cursor.y() >= dragStart.y()) {
		mat(0,1) = dragStart.y();
		mat(1,1) = cursor.y();
	} else {
		mat(0,1) = cursor.y();
		mat(1,1) = dragStart.y();
	}
	
	return mat;
}
