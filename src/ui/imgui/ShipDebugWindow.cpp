/*
 * ShipDebugWindow.cpp
 *
 *  Created on: Apr 22, 2021
 *      Author: exuvo
 */

#include <Refureku/Refureku.h>

#include "ShipDebugWindow.hpp"
#include "Aurora.hpp"
#include "galaxy/Player.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/ShadowStarSystem.hpp"

template<typename T>
void printField(const rfk::Field& f, T& instance) {
	const rfk::Type& t = f.getType();
	std::ostringstream osValue;
	
	if (t.isValue()) {
		
		if (t == rfk::getType<float>()) {
			osValue << f.get<float>(instance);
		} else if (t == rfk::getType<double>()) {
			osValue << f.get<double>(instance);
			
		} else if (t == rfk::getType<int8_t>()) {
			osValue << f.get<int8_t>(instance);
		} else if (t == rfk::getType<int16_t>()) {
			osValue << f.get<int16_t>(instance);
		} else if (t == rfk::getType<int32_t>()) {
			osValue << f.get<int32_t>(instance);
		} else if (t == rfk::getType<int64_t>()) {
			osValue << f.get<int64_t>(instance);
			
		} else if (t == rfk::getType<uint8_t>()) {
			osValue << f.get<uint8_t>(instance);
		} else if (t == rfk::getType<uint16_t>()) {
			osValue << f.get<uint16_t>(instance);
		} else if (t == rfk::getType<uint32_t>()) {
			osValue << f.get<uint32_t>(instance);
		} else if (t == rfk::getType<uint64_t>()) {
			osValue << f.get<uint64_t>(instance);
			
		} else if (t == rfk::getType<bool>()) {
			osValue << f.get<bool>(instance);
		} else if (t == rfk::getType<char>()) {
			osValue << f.get<char>(instance);
		} else {
			osValue << "unknown value type";
		}
		
	} else if (t.isCArray()) {
		
		if (t.getArchetype() == rfk::getArchetype<char>()) {
			osValue << static_cast<const char*>(f.getConstPtr(instance));
		} else {
			osValue << "unknown [] type";
		}
		
	} else if (t.isPointer()) {
		
		osValue << "unknown * type";
		
	} else {
		osValue << "unknown";
	}
	
	std::ostringstream osType;
	rfk::Archetype const* at = t.getArchetype();
	
	if (at != nullptr) {
		osType << at->getName();
		
		if (t.isCArray()) {
			osType << "[" << t.getCArraySize() << "]";
		}
		
		if (at->getKind() == rfk::EEntityKind::Enum) {
			osValue << " enum";
		} else if (at->getKind() == rfk::EEntityKind::EnumValue) {
			osValue << " enum value";
		} else if (at->getKind() == rfk::EEntityKind::Class) {
			osValue << " class";
		} else if (at->getKind() == rfk::EEntityKind::Struct) {
			osValue << " struct";
		} else if (at->getKind() == rfk::EEntityKind::Field) {
			osValue << " field";
		} else if (at->getKind() == rfk::EEntityKind::Variable) {
			osValue << " variable";
		} else if (at->getKind() == rfk::EEntityKind::Method) {
			osValue << " method";
		} else if (at->getKind() == rfk::EEntityKind::Function) {
			osValue << " function";
		}
		
	} else {
		osType << "?";
	}
	
	ImGui::Text(" %s: %s = %s", f.getName(), osType.str().data(), osValue.str().data());
}

template<typename T>
void printComponent(entt::registry& registry, entt::entity entityID) {
	T* c = registry.try_get<T>(entityID);
	if (c != nullptr) {
		rfk::Struct const& s = T::staticGetArchetype();
		ImGui::Text("%s, fields %lu, %lu bytes", s.getName(), s.getFieldsCount(), s.getMemorySize());
		
		auto visitor = [](rfk::Field const& f, void* c) -> bool {
			printField(f, *static_cast<T*>(c));
			return true;
		};
		s.foreachField(visitor, c, true);
	}
}

void ShipDebugWindow::render() {
	if (ImGui::Begin("Ship debug", &visible, ImGuiWindowFlags_AlwaysAutoResize)) {
						
			if (Aurora.settings.render.useShadow) {
				if (ImGui::Button("Shadow")) {
					Aurora.settings.render.useShadow = false;
				}
			} else {
				if (ImGui::Button(" Live ")) {
					Aurora.settings.render.useShadow = true;
				}
			}
			ImGui::SameLine();
			
			auto selectedEntities = Player::current->selection;

			if (selectedEntities.size() == 0) {
				ImGui::TextUnformatted("Nothing selected");

			} else {
				
				if (selectionIndex > selectedEntities.size() - 1) {
					selectionIndex = 0;
				}
				
				uint32_t sliderMin = 0;
				uint32_t sliderMax = selectedEntities.size() - 1;
				char txt[50];
				snprintf(txt, 50, "%u / %lu", 1 + selectionIndex, selectedEntities.size());
				ImGui::SliderScalar("Selection", ImGuiDataType_U32, &selectionIndex, &sliderMin, &sliderMax, txt, ImGuiSliderFlags_AlwaysClamp);
//				ImGui::SliderScalar("Selection", ImGui::DataType.Int, ::selectionIndex, 0, selectedEntities.size() - 1, "${1 + selectionIndex} / ${selectedEntities.size()}", 2.0f)
				
				EntityReference* entityRef = &selectedEntities[selectionIndex];
				
				if (Aurora.settings.render.useShadow) {
					if (!entityRef->isValid(*entityRef->system->shadow)) {
						if (entityRef->resolveReference(*entityRef->system->shadow)) {
							Player::current->replaceSelection(*entityRef, selectionIndex);
						} else {
							std::cout << "Invalid entity reference " << entityRef << std::endl;
							Player::current->removeSelection(*entityRef);
							entityRef = nullptr;
						}
					}
				} else {
					if (!entityRef->isValid(*entityRef->system)) {
						if (entityRef->resolveReference(*entityRef->system)) {
							Player::current->replaceSelection(*entityRef, selectionIndex);
						} else {
							std::cout << "Invalid entity reference " << entityRef << std::endl;
							Player::current->removeSelection(*entityRef);
							entityRef = nullptr;
						}
					}
				}
				
				if (entityRef != nullptr) {
					
					StarSystem& system = *entityRef->system;
					entt::registry& registry = Aurora.settings.render.useShadow ? system.shadow->registry : system.registry;
					
					entt::entity entityID = entityRef->entityID;
					
					ShipComponent* ship = registry.try_get<ShipComponent>(entityID);
//					PartStatesComponent& partStates = registry.get<PartStatesComponent>(entityID);
//					ShieldComponent& shield = registry.get<ShieldComponent>(entityID);
//					ArmorComponent& armor = registry.get<ArmorComponent>(entityID);
//					PartsHPComponent& partsHP = registry.get<PartsHPComponent>(entityID);
					CargoComponent* cargoC = registry.try_get<CargoComponent>(entityID);
					ImGui::Text("Entity ID %d", entityRef->entityID);
					
					if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen)) { // ImGuiTreeNodeFlags_DefaultOpen

						registry.visit(entityID, [&](const entt::type_info type){
//							ImGui::Text("%s", type.name().data());
						  auto storage = registry.storage(type);
						});
						
#define INSPECT_COMPONENTS_TEMPLATE(r, unused, component) printComponent<component>(registry, entityID);
						
						BOOST_PP_SEQ_FOR_EACH(INSPECT_COMPONENTS_TEMPLATE, ~, SYNCED_COMPONENTS_SEQ);
						
//						components.sort({ o1, o2 ->
//							o1::class.simpleName!!.compareTo(o2::class.simpleName!!)
//						})
//						
//						components.forEachFast{ component ->
//
//							if (treeNode(component::class.java.simpleName)) {
//
//								val fields = ReflectionUtils.getFields(component::class.java)
//								
//								fields.forEachFast{ field ->
//									ReflectionUtils.fixFieldAccess(field)
//									
//									if (Collection::class.java.isAssignableFrom(field.getType())) {
//										val collection = field.get(component) as Collection<*>
//										
//										if (treeNode("${field.name}: ${collection.size}")) {
//											for(item in collection) {
//												textUnformatted("$item")
//											}
//											treePop()
//										}
//									} else {
//										textUnformatted("${field.name}: ${field.get(component)}")
//									}
//								}
//
//								treePop()
//							}
//						}
					}

//					if (ship != null) {
//
//						if (collapsingHeader("Parts", 0)) { // TreeNodeFlag.DefaultOpen.i
//							
//							sliderScalar("Weapon test range", DataType.Double, ::weaponTestDistance, 100.0, Units.AU * 1000, Units.distanceToString(weaponTestDistance.toLong()), 8.0f)
//
//							ship.hull.getPartRefs().forEachFast{ partRef ->
//								if (treeNode("${partRef.part::class.simpleName} ${partRef.part.name}")) {
//
//									if (partRef.part is PoweringPart) {
//										val state = partStates[partRef][PoweringPartState::class]
//										textUnformatted("availablePower ${Units.powerToString(state.availiablePower)}")
//										textUnformatted("producedPower ${Units.powerToString(state.producedPower)}")
//									}
//
//									if (partRef.part is PoweredPart) {
//										val state = partStates[partRef][PoweredPartState::class]
//										textUnformatted("requestedPower ${Units.powerToString(state.requestedPower)}")
//										textUnformatted("givenPower ${Units.powerToString(state.givenPower)}")
//									}
//
//									if (partRef.part is ChargedPart) {
//										val state = partStates[partRef][ChargedPartState::class]
//										textUnformatted("charge ${Units.powerToString(state.charge)}")
//										textUnformatted("expectedFullAt ${Units.secondsToString(state.expectedFullAt)}")
//									}
//
//									if (partRef.part is PassiveSensor) {
//										val state = partStates[partRef][PassiveSensorState::class]
//										textUnformatted("lastScan ${state.lastScan}")
//									}
//
//									if (partRef.part is AmmunitionPart) {
//										val state = partStates[partRef][AmmunitionPartState::class]
//										textUnformatted("type ${state.type?.name}")
//										textUnformatted("amount ${state.amount}/${partRef.part.ammunitionAmount}")
//										textUnformatted("reloadedAt ${Units.secondsToString(state.reloadedAt)}")
//									}
//
//									if (partRef.part is FueledPart) {
//										val state = partStates[partRef][FueledPartState::class]
//										textUnformatted("fuelEnergyRemaining ${state.fuelEnergyRemaining}")
//										textUnformatted("totalFuelEnergyRemaining ${state.totalFuelEnergyRemaining}")
//									}
//
//									if (partRef.part is TargetingComputer) {
//										
//										val state = partStates[partRef][TargetingComputerState::class]
//										textUnformatted("target ${state.target?.entityID}")
//										textUnformatted("lockCompletionAt ${state.lockCompletionAt}")
//										
//										if (treeNode("linkedWeapons ${state.linkedWeapons.size()}###linked")) {
//											for(weaponRef in state.linkedWeapons) {
//												textUnformatted("$weaponRef")
//											}
//											treePop()
//										}
//										
//										if (treeNode("readyWeapons ${state.readyWeapons.size()}###ready")) {
//											for(weaponRef in state.readyWeapons) {
//												textUnformatted("$weaponRef")
//											}
//											treePop()
//										}
//										
//										if (treeNode("chargingWeapons ${state.chargingWeapons.size}###charging")) {
//											for(weaponRef in state.chargingWeapons) {
//												textUnformatted("$weaponRef")
//											}
//											treePop()
//										}
//										
//										if (treeNode("reloadingWeapons ${state.reloadingWeapons.size}###reloading")) {
//											for(weaponRef in state.reloadingWeapons) {
//												textUnformatted("$weaponRef")
//											}
//											treePop()
//										}
//										
//										if (treeNode("disabledWeapons ${state.disabledWeapons.size()}###disabled")) {
//											for(weaponRef in state.disabledWeapons) {
//												textUnformatted("$weaponRef")
//											}
//											treePop()
//										}
//										
//									} else if (partRef.part is BeamWeapon) {
//										
//										val weaponTestDistanceL = weaponTestDistance.toLong()
//										
//										textUnformatted("radialDivergence ${partRef.part.getRadialDivergence() * 1000} mrad")
//										textUnformatted("beamRadiusAtDistance ${partRef.part.getBeamRadiusAtDistance(weaponTestDistanceL)} m")
//										textUnformatted("beamArea ${partRef.part.getBeamArea(weaponTestDistanceL)} m²")
//										textUnformatted("deliveredEnergyTo1MSquareAtDistance ${Units.powerToString(partRef.part.getDeliveredEnergyTo1MSquareAtDistance(weaponTestDistanceL))}")
//										
//										val projectileSpeed = Units.C * 1000
//										val timeToIntercept = FastMath.ceil(weaponTestDistance / projectileSpeed).toLong()
//										val galacticTime = timeToIntercept + galaxy.time
//										val galacticDays = (galacticTime / (60 * 60 * 24)).toInt()
//										
//										textUnformatted("projectileSpeed $projectileSpeed m/s")
//										textUnformatted("timeToIntercept ${timeToIntercept} s, at ${Units.daysToDate(galacticDays)} ${Units.secondsToString(galacticTime)}")
//										
//									} else if (partRef.part is Railgun) {
//										
//										val ammoState = partStates[partRef][AmmunitionPartState::class]
//										
//										val munitionClass = ammoState.type as? SimpleMunitionHull
//										
//										if (munitionClass != null) {
//										
//											val projectileSpeed = (partRef.part.capacitor * partRef.part.efficiency) / (100L * munitionClass.loadedMass)
//											val weaponTestDistance = weaponTestDistance.toLong()
//											val timeToIntercept = FastMath.max(1, weaponTestDistance / projectileSpeed)
//											val galacticTime = timeToIntercept + galaxy.time
//											val galacticDays = (galacticTime / (60 * 60 * 24)).toInt()
//											
//											textUnformatted("projectileSpeed $projectileSpeed m/s")
//											textUnformatted("timeToIntercept ${timeToIntercept} s, at ${Units.daysToDate(galacticDays)} ${Units.secondsToString(galacticTime)}")
//										}
//										
//									} else if (partRef.part is MissileLauncher) {
//										
//										val ammoState = partStates[partRef][AmmunitionPartState::class]
//										
//										val munitionClass = ammoState.type as? AdvancedMunitionHull
//										
//										if (munitionClass != null) {
//											
//											val missileAcceleration = munitionClass.getAverageAcceleration()
//											val missileLaunchSpeed = partRef.part.launchForce / munitionClass.loadedMass
//											
//											textUnformatted("launchSpeed $missileLaunchSpeed m/s + acceleration ${missileAcceleration} m/s²")
//											
//											val a: Double = missileAcceleration.toDouble() / 2
//											val b: Double = missileLaunchSpeed.toDouble()
//											val c: Double = -weaponTestDistance
//											
//											val timeToIntercept = FastMath.ceil(WeaponSystem.getPositiveRootOfQuadraticEquation(a, b, c)).toLong()
//											
//											val galacticTime = timeToIntercept + galaxy.time
//											val galacticDays = (galacticTime / (60 * 60 * 24)).toInt()
//											val impactVelocity = missileLaunchSpeed + missileAcceleration * FastMath.min(timeToIntercept, munitionClass.thrustTime.toLong())
//											
//											textUnformatted("impactVelocity $impactVelocity m/s")
//											textUnformatted("timeToIntercept $timeToIntercept s / thrustTime ${munitionClass.thrustTime} s")
//											textUnformatted("interceptAt ${Units.daysToDate(galacticDays)} ${Units.secondsToString(galacticTime)}")
//										}
//									}
//
//									treePop()
//								}
//							}
//						}
//						
//						if (collapsingHeader("Health", 0)) { // TreeNodeFlag.DefaultOpen.i
//							
//							spacing()
//							
//							group {
//								if (shield != null) {
//									if (ShipUI.shieldBar(ship, shield)) {
//										tooltip {
//											ship.hull.shields.forEachFast { partRef ->
//												val charge = partStates[partRef][ChargedPartState::class].charge
//												val capacity = (partRef.part as Shield).capacitor
//												
//												val text = "${partRef.part.name} ${Units.capacityToString(charge)} / ${Units.capacityToString(capacity)}"
//												
//												if (!partStates.isPartEnabled(partRef)) {
//													
//													withStyleColor(Col.Text, ShipUI.emptyArmorColor) {
//														textUnformatted(text)
//													}
//													
//												} else {
//													textUnformatted(text)
//												}
//											}
//										}
//									}
//									currentWindow.dc.cursorPos.y += 2
//								}
//								
//								ShipUI.armor(ship, armor, { _, y, armorHP, maxArmorHP ->
//									setTooltip("$armorHP / $maxArmorHP, resistance ${ship.hull.armorEnergyPerDamage[y]}")
//								})
//							}
//							sameLine()
//							group {
//								if (shield != null) {
//									text("%s / %s", Units.capacityToString(shield.shieldHP), Units.capacityToString(ship.hull.maxShieldHP))
//								}
//								textUnformatted("ArmorHP ${armor.getTotalHP()} / ${ship.hull.maxArmorHP}")
//								textUnformatted("PartHP ${partsHP.totalPartHP} / ${ship.hull.maxPartHP}")
//							}
//							
//							val sortedParts = Bag<PartRef<Part>>(ship.hull.getPartRefs().size)
//							ship.hull.getPartRefs().forEachFast{ partRef ->
//								sortedParts.add(partRef)
//							}
//							sortedParts.sort{ p1, p2 ->
//								val hitChance1 = (100 * p1.part.volume) / ship.hull.volume
//								val hitChance2 = (100 * p2.part.volume) / ship.hull.volume
//								
//								(hitChance2 - hitChance1).toInt()
//							}
//							
//							sortedParts.forEachFast{ partRef ->
//								val hitChance = (100 * partRef.part.volume) / ship.hull.volume
//								textUnformatted("${partsHP.getPartHP(partRef)} / ${partRef.part.maxHealth.toInt()} ${String.format("%3d", hitChance)}% ${partRef.part}")
//							}
//							
//							sliderScalar("Damage amount", DataType.Long, ::testDmgAmount, 0L, 1_000_000L, "$testDmgAmount", 2.5f)
//							
//							val ImGui::ButtonFlags = if (useShadow) ButtonFlag.Disabled.i else 0
//							
//							if (ImGui::ButtonEx("damage", Vec2(), ImGui::ButtonFlags)) {
//								val weaponSystem = world.getSystem(WeaponSystem::class.java)
//								weaponSystem.applyDamage(entityRef.entityID, testDmgAmount, DamagePattern.LASER)
//								system.skipClearShadowChanged = true
//							}
//							
//							if (ImGui::ButtonEx("kill armor", Vec2(), ImGui::ButtonFlags)) {
//								for (y in 0 until ship.hull.armorLayers) {
//									for (x in 0 until ship.hull.getArmorWidth()) {
//										armor[y][x] = 0u
//									}
//								}
//								
//								system.changed(entityID, armorMapper)
//								system.skipClearShadowChanged = true
//							}
//							
//							if (ImGui::ButtonEx("repair", Vec2(), ImGui::ButtonFlags)) {
//								for (y in 0 until ship.hull.armorLayers) {
//									for (x in 0 until ship.hull.getArmorWidth()) {
//										armor[y][x] = ship.hull.armorBlockHP[y]
//									}
//								}
//								
//								for (partRef in ship.hull.getPartRefs()) {
//									partsHP.setPartHP(partRef, partRef.part.maxHealth.toInt())
//								}
//								
//								system.changed(entityID, armorMapper, partsHPMapper)
//								system.skipClearShadowChanged = true
//							}
//						}
//
//						if (collapsingHeader("Power", 0)) {
//
//							val solarIrradiance = irradianceMapper.get(entityRef.entityID)
//
//							if (solarIrradiance != null) {
//
//								textUnformatted("Solar irradiance ${solarIrradiance.irradiance} W/m2")
//							}
//
//							val powerComponent = powerMapper.get(entityRef.entityID)
//
//							if (powerComponent != null) {
//
//								separator()
//
//								val now = System.currentTimeMillis()
//
//								if (now - lastDebugTime > 500) {
//									lastDebugTime = now
//
//									powerAvailiableValues[arrayIndex] = powerComponent.totalAvailablePower.toFloat()
//									powerRequestedValues[arrayIndex] = powerComponent.totalRequestedPower.toFloat()
//									powerUsedValues[arrayIndex] = powerComponent.totalUsedPower.toFloat() / powerComponent.totalAvailablePower.toFloat()
//									arrayIndex++
//
//									if (arrayIndex >= 60) {
//										arrayIndex = 0
//									}
//								}
//
//								plotLines("AvailablePower", { powerAvailiableValues[(arrayIndex + it) % 60] }, 60, 0, "", 0f, Float.MAX_VALUE, Vec2(0, 50))
//								plotLines("RequestedPower", { powerRequestedValues[(arrayIndex + it) % 60] }, 60, 0, "", 0f, Float.MAX_VALUE, Vec2(0, 50))
//								plotLines("UsedPower", { powerUsedValues[(arrayIndex + it) % 60] }, 60, 0, "", 0f, 1f, Vec2(0, 50))
//
//								if (treeNode("Producers")) {
//									powerComponent.poweringParts.forEach({
//										val partRef = it
//										val poweringState = partStates[partRef][PoweringPartState::class]
//
//										val power = if (poweringState.availiablePower == 0L) 0f else poweringState.producedPower / poweringState.availiablePower.toFloat()
//
//										progressBar(power, Vec2(), "${Units.powerToString(poweringState.producedPower)}/${Units.powerToString(poweringState.availiablePower)}")
//
//										sameLine(0f, style.itemInnerSpacing.x)
//										textUnformatted("${partRef.part}")
//
//										if (partRef is FueledPart && partRef is PoweringPart) {
//
//											val fueledState = partStates[partRef][FueledPartState::class]
//											val fuelRemaining = Units.secondsToString(fueledState.fuelEnergyRemaining / partRef.power)
//											val totalFuelRemaining = Units.secondsToString(fueledState.totalFuelEnergyRemaining / partRef.power)
//
//											textUnformatted("Fuel $fuelRemaining/$totalFuelRemaining W")
//										}
//
//										if (partRef.part is Battery) {
//
//											val chargedState = partStates[partRef][ChargedPartState::class]
//											val charge = chargedState.charge
//											val maxCharge = partRef.part.capacitor
//											val charged = if (maxCharge == 0L) 0f else charge / maxCharge.toFloat()
//
//											progressBar(charged, Vec2(), "${Units.powerToString(charge)}/${Units.powerToString(maxCharge)}s")
//
//											if (poweringState.producedPower > 0L) {
//
//												sameLine(0f, style.itemInnerSpacing.x)
//												textUnformatted(Units.secondsToString(charge / poweringState.producedPower))
//											}
//										}
//									})
//
//									treePop()
//								}
//
//								if (treeNode("Consumers")) {
//									powerComponent.poweredParts.forEach({
//										val part = it
//										val poweredState = partStates[part][PoweredPartState::class]
//
//										val power = if (poweredState.requestedPower == 0L) 0f else poweredState.givenPower / poweredState.requestedPower.toFloat()
//										progressBar(power, Vec2(), "${Units.powerToString(poweredState.givenPower)}/${Units.powerToString(poweredState.requestedPower)}")
//
//										sameLine(0f, style.itemInnerSpacing.x)
//										textUnformatted("${part.part}")
//									})
//
//									treePop()
//								}
//							}
//						}
//
//						if (collapsingHeader("Cargo", 0)) { //TreeNodeFlags.DefaultOpen.i
//
//							CargoType.values().forEach {
//								val cargo = it
//
//								val usedVolume = cargoC.getUsedCargoVolume(cargo)
//								val maxVolume = cargoC.getMaxCargoVolume(cargo)
//								val usedMass = cargoC.getUsedCargoMass(cargo)
//								val usage = if (maxVolume == 0L) 0f else usedVolume / maxVolume.toFloat()
//								progressBar(usage, Vec2(), "$usedMass kg, ${usedVolume / 1000}/${maxVolume / 1000} m³")
//
//								sameLine(0f, style.itemInnerSpacing.x)
//								textUnformatted(cargo.name)
//
//								if (cargo == CargoType.AMMUNITION) {
//									val munitions = cargoC.munitions
//									if (munitions != null) {
//										for (entry in munitions.entries) {
//											textUnformatted("${entry.value} ${entry.key}")
//										}
//									}
//								}
//							}
//
//							separator();
//
//							if (beginCombo("", addResource.name)) { // The second parameter is the label previewed before opening the combo.
//								for (resource in Resource.values()) {
//									val isSelected = addResource == resource
//
//									if (selectable(resource.name, isSelected)) {
//										addResource = resource
//									}
//
//									if (isSelected) { // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
//										setItemDefaultFocus()
//									}
//								}
//								endCombo()
//							}
//
//							inputScalar("kg", DataType.Int, ::addResourceAmount, 10, 100, "%d", 0)
//							
//							val ImGui::ButtonFlags = if (useShadow) ButtonFlag.Disabled.i else 0
//							
//							if (ImGui::ButtonEx("Add", Vec2(), ImGui::ButtonFlags)) {
//								if (!cargoC.addCargo(addResource, addResourceAmount.toLong())) {
//									println("Cargo does not fit")
//								}
//							}
//
//							sameLine(0f, style.itemInnerSpacing.x)
//							
//							if (ImGui::ButtonEx("Remove", Vec2(), ImGui::ButtonFlags)) {
//								if (cargoC.retrieveCargo(addResource, addResourceAmount.toLong()) != addResourceAmount.toLong()) {
//									println("Does not have enough of specified cargo")
//								}
//							}
//
//							if (treeNode("Each resource")) {
//								Resource.values().forEach {
//									val resource = it
//
//									val usedVolume = cargoC.getUsedCargoVolume(resource)
//									val maxVolume = cargoC.getMaxCargoVolume(resource)
//									val usedMass = cargoC.getUsedCargoMass(resource)
//									val usage = if (maxVolume == 0L) 0f else usedVolume / maxVolume.toFloat()
//									progressBar(usage, Vec2(), "$usedMass kg, ${usedVolume / 1000}/${maxVolume / 1000} m³")
//
//									sameLine(0f, style.itemInnerSpacing.x)
//									textUnformatted(resource.name)
//								}
//
//								treePop()
//							}
//						}
//					}
				}
			}
	}
	
	ImGui::End();
}
