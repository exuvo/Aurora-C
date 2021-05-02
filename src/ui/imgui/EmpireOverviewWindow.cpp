/*
 * EmpireOverviewWindow.cpp
 *
 *  Created on: Jan 1, 2021
 *      Author: exuvo
 */

#include <imgui.h>
#include <imgui_internal.h>

#include "Aurora.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "galaxy/Player.hpp"
#include "starsystems/StarSystem.hpp"
#include "starsystems/ShadowStarSystem.hpp"
#include "EmpireOverviewWindow.hpp"
#include "utils/RenderUtils.hpp"
#include "utils/ImGuiUtils.hpp"

void EmpireOverviewWindow::render() {
	ImGuiStyle& style = ImGui::GetStyle();
	float itemSpaceX = style.ItemSpacing.x;
	float itemSpaceY = style.ItemSpacing.y;
	
	Empire* empire = Player::current->empire;
	
	if (empire != nullptr) {
		
//		ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
		ImVec2 viewportPos = ImGui::GetCurrentContext()->CurrentViewport->Pos;
		
//		std::cout << "viewportPos " << viewportPos.x << " " << viewportPos.y << std::endl;
		
		ImGui::SetNextWindowSize({ 150, layer.getMainWindow().window->GetSize().y - 50 });
		ImGui::SetNextWindowPos(viewportPos + ImVec2{ 0, 20 });
		ImGui::SetNextWindowBgAlpha(0.4f);
		if (ImGui::Begin("Empire Overview", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize)) {
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			
			bool windowCoveredByOtherWindow = false;
			ImRect& thisWindowBB = window->OuterRectClipped;
			
			for (int i = 1; i < ImGui::GetCurrentContext()->Windows.size(); i++) { // 0 is fallback window
				ImGuiWindow* win = ImGui::GetCurrentContext()->Windows[i];
				if (win != window && ((win->Active && !win->Hidden) || (win->WasActive && (win->Flags & ImGuiWindowFlags_::ImGuiWindowFlags_Tooltip)))) {
					ImRect& bb = win->OuterRectClipped;
					
					if (bb.Overlaps(thisWindowBB)) {
						windowCoveredByOtherWindow = true;
						break;
					}
				}
			}
			
			/*
					View modes
						relative: current system at top, rest sorted by distance from current
						heliocentric: home system first, rest sorted by distance from home
						
					system tree
						name
						force overview dots (all factions)
						
						repeat for us, enemy, friendly, neutral
							planets (only colonised)
							stations
							large ships
							small ships
				*/
			
			void* strategicIconsTexture = nullptr; //renderSystemGlobalData.strategicIconTexture
			inlineIcons.clear();
			
			for (StarSystem* system : Player::current->visibleSystems) {
				if (ImGui::TreeNodeEx(std::to_string((entt::id_type) system->galacticEntityID).c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen, system->name.c_str())) {
					
					ShadowStarSystem& shadow = *system->shadow;
					
					auto drawIcon = [&](entt::entity entityID, StrategicIconComponent& icon, bool selected) {
						if (window->SkipItems) {
							return;
						}
						
						ImVec2 size = {17, 17};
						ImRect bb = {window->DC.CursorPos, window->DC.CursorPos + size};
						
						ImGui::ItemSize(bb);
						if (!ImGui::ItemAdd(bb, ImGui::GetID("$entityID"))) {
							return;
						}
						
						if (ImGui::IsItemHovered()) {
							window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(toLinearRGB(ImVec4(0.8f, 0.8f, 0.8f, 1.0f))), 0.0f);
							
						} else if (selected) {
							window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(toLinearRGB(ImVec4(0.5f, 0.5f, 0.5f, 1.0f))), 0.0f);
						}
						
						if (selected) {
							window->DrawList->AddRectFilled(bb.Min + 1, bb.Max - 1, ImGui::GetColorU32(toLinearRGB(ImVec4(0.3f, 0.3f, 0.3f, 0.1f))), 0.0f);
						}
						
						EmpireComponent* empireC = shadow.registry.try_get<EmpireComponent>(entityID);
						
						vk2d::Colorf color;
						if (empireC != nullptr) {
							color = empireC->empire->color;
						} else {
							TintComponent* tintC = shadow.registry.try_get<TintComponent>(entityID);;
							color = tintC != nullptr ? tintC->color : vk2d::Colorf::WHITE();
						}
						color = sRGBtoLinearRGB(color);
						
//						if (icon.baseTexture->texture == strategicIconsTexture) {
//							
							if (!windowCoveredByOtherWindow) {
//								// Batched rendering of strategic icons after imgui has been drawn via postDraw
//								deferredIcons.push_back(DeferredIconDraw{ icon, bb, ImGui::GetColorU32(ImVec4(color.r, color.g, color.g, color.a)) });
//								
//							} else {
//								inlineIcons.push_back(InlineIconDraw{ icon, bb, ImGui::GetColorU32(ImVec4(color.r, color.g, color.g, color.a)), this });
//								
//								// Inefficient but renders in correct order
//								window->DrawList->AddCallback(static_cast<ImDrawCallback>([](const ImDrawList* parent_list, const ImDrawCmd* cmd) {
//									InlineIconDraw* deferredIcon = (InlineIconDraw*) cmd->UserCallbackData;
//									
//									deferredIcon->window->inlineDraw(*deferredIcon);
//									
//								}), (void*) &inlineIcons.back());
							}
//							
//						} else {
//							window->DrawList.AddImage(icon.baseTexture->getTexture().textureObjectHandle, bb.Min + 1, bb.Max - 1, ImVec2(icon.baseTexture->u, icon.baseTexture->v), ImVec2(icon.baseTexture->u2, icon.baseTexture->v2), ImGui::GetColorU32(ImVec4(color.r, color.g, color.g, color.a)));
//						}
					};
					
					std::unique_lock<LockableBase(std::mutex)> lock(Aurora.galaxy->shadowLock);
					
					std::vector<EntityReference> systemColonies(empire->colonies.size());
					
					for (EntityReference& ref : empire->colonies) {
						if (ref.system == system) {
							systemColonies.push_back(ref);
						}
					}
					
					ImGui::BeginGroup();
					{
//						window->dc.layoutType = LayoutType.Horizontal;
//						pushStyleVar(StyleVar.ItemSpacing, ImVec2(0,0));
//						
						for (EntityReference& colonyRef : systemColonies) {
							
							entt::entity entityID = colonyRef.entityID;
							
							NameComponent* nameC = shadow.registry.try_get<NameComponent>(entityID);
							ColonyComponent& colony = shadow.registry.get<ColonyComponent>(entityID);
							StrategicIconComponent& icon = shadow.registry.get<StrategicIconComponent>(entityID);
//							
//							drawIcon(entityID, icon, isSelected(entityID, system));
//							
//							if (isItemHovered()) {
//								popStyleVar();
//								tooltip {
//									textUnformatted("$name");
//									textUnformatted("Population: ${colony.population}");
//								}
//								pushStyleVar(StyleVar.ItemSpacing, ImVec2(0,0));
//							}
//							
//							if (isItemClicked()) {
//								if (Player.current.selection.isNotEmpty() && !Gdx.input.isKeyPressed(Input.Keys.SHIFT_LEFT)) {
//									Player.current.selection.clear();
//								}
//								
//								Player.current.selection.add(shadow.getEntityReference(entityID));
//							}
//							
//							if (window->dc.cursorPos.x + 17 > windowContentRegionWidth) {
//								newLine();
//							}
						}
						
//						popStyleVar();
//						window->dc.layoutType = LayoutType.Vertical;
//						ImGui::EndGroup();
					}
					
					ImGui::BeginGroup();
					{
//						window->dc.layoutType = LayoutType.Horizontal
//						pushStyleVar(StyleVar.ItemSpacing, ImVec2(0,0))
//						
//						shadow.empireShips[empire]?.values?.forEach { bag ->
//							bag.forEachFast { index, entityID ->
//
//								val name = nameMapper.get(entityID).name
//								val ship = shipMapper.get(entityID)
//								val icon = strategicIconMapper.get(entityID)
//								
//								drawIcon(entityID, icon, isSelected(entityID, system))
//								
//								//TODO shield, armor, health bars
//								
//								if (isItemHovered()) {
//									popStyleVar()
//									tooltip {
//										textUnformatted("$name - ${ship.hull}")
//										
//										val powerMapper = shadow.powerMapper
//										val shield = shadow.shieldMapper.get(entityID)
//										val armor = shadow.armorMapper.get(entityID)
//										val partsHP = shadow.partsHPMapper.get(entityID)
//										val cargoC = shadow.cargoMapper.get(entityID)
//										
//										group {
//											if (shield != null) {
//												ShipUI.shieldBar(ship, shield)
//												currentWindow.dc.cursorPos.y += 2
//											}
//											ShipUI.armor(ship, armor, null)
//										}
//										sameLine()
//										group {
//											if (shield != null) {
//												text("%s / %s", Units.capacityToString(shield.shieldHP), Units.capacityToString(ship.hull.maxShieldHP))
//											}
//											textUnformatted("ArmorHP ${armor.getTotalHP()} / ${ship.hull.maxArmorHP}")
//											textUnformatted("PartHP ${partsHP.totalPartHP} / ${ship.hull.maxPartHP}")
//										}
//										
//										val sortedParts = Bag<PartRef<Part>>(ship.hull.getPartRefs().size)
//										for(partRef in ship.hull.getPartRefs()) {
//											val hp = partsHP.getPartHP(partRef)
//											val maxHP = partRef.part.maxHealth.toInt()
//											if (hp < maxHP) {
//												sortedParts.add(partRef)
//											}
//										}
//										
//										sortedParts.sort{ p1, p2 ->
//											var diff = p2.part.maxHealth.toInt() - p1.part.maxHealth.toInt()
//											
//											if (diff == 0) {
//												diff = partsHP.getPartHP(p2) - partsHP.getPartHP(p1)
//											}
//											
//											diff
//										}
//										
//										sortedParts.forEachFast{ partRef ->
//											val hp = partsHP.getPartHP(partRef)
//											val maxHP = partRef.part.maxHealth.toInt()
//											
//											if (hp == 0) {
//												pushStyleColor(Col.Text, ImVec4(1, 0, 0, 1))
//											} else {
//												pushStyleColor(Col.Text, ImVec4(0.5, 0.5, 0, 1))
//											}
//											
//											textUnformatted("$hp / $maxHP ${partRef.part}")
//											popStyleColor()
//										}
//									}
//									pushStyleVar(StyleVar.ItemSpacing, ImVec2(0,0))
//								}
//								
//								if (isItemClicked()) {
//									if (Player.current.selection.isNotEmpty() && !Gdx.input.isKeyPressed(Input.Keys.SHIFT_LEFT)) {
//										Player.current.selection.clear()
//									}
//									
//									Player.current.selection.add(shadow.getEntityReference(entityID))
//								}
//								
//								if (window->dc.cursorPos.x + 17 > windowContentRegionWidth) {
//									newLine()
//								}
//							}
//						}
//						
//						popStyleVar()
//						window->dc.layoutType = LayoutType.Vertical
						ImGui::EndGroup();
					}
					
					//TODO show other factions
				}
			}
		}
		ImGui::End();
	}
}


void EmpireOverviewWindow::inlineDraw(DeferredIconDraw& deferredIcon) {
//	val projectionMatrix = imguiCamera.combined
//	
//	val texture = renderSystemGlobalData.strategicIconTexture
//	val vertices = renderSystemGlobalData.strategicIconVertices
//	val indices = renderSystemGlobalData.strategicIconIndices
//	val iconShader = renderSystemGlobalData.strategicIconShader
//	val mesh = renderSystemWindowData.strategicIconMesh
//	
//	val lastProgram = Gdx.gl.glGetIntegerv(GL20.GL_CURRENT_PROGRAM, glParam).let{ glParam[0] }
//	val lastArrayBuffer = Gdx.gl.glGetIntegerv(GL20.GL_ARRAY_BUFFER_BINDING, glParam).let { glParam[0] }
//	val lastVertexArray = Gdx.gl.glGetIntegerv(GL30.GL_VERTEX_ARRAY_BINDING, glParam).let{ glParam[0] }
//	val lastElementBuffer = Gdx.gl.glGetIntegerv(GL20.GL_ELEMENT_ARRAY_BUFFER_BINDING, glParam).let{ glParam[0] }
//	
//	iconShader.bind()
//	iconShader.setUniformMatrix("u_projTrans", projectionMatrix);
//	iconShader.setUniformi("u_texture", 14);
//	
//	texture.bind()
//	
//	var vertexIdx = 0
//	var indiceIdx = 0
//
//	// Offset center texCoords by diff in texture sizes. 7 15 3+1+3 7+1+7 7-3=4
//	val centerXOffset = 4f / texture.width
//	val centerYOffset = 4f / texture.height
//
//	fun vertex(x: Float, y: Float, colorBits: Float, baseU: Float, baseV: Float, centerU: Float, centerV: Float) {
//		vertices[vertexIdx++] = x;
//		vertices[vertexIdx++] = y;
//		vertices[vertexIdx++] = colorBits
//		vertices[vertexIdx++] = baseU;
//		vertices[vertexIdx++] = baseV;
//		vertices[vertexIdx++] = centerU;
//		vertices[vertexIdx++] = centerV;
//	}
//
//	val baseTex = deferredIcon.icon!!.baseTexture
//	val centerTex = deferredIcon.icon!!.centerTexture
//	val colorBits = deferredIcon.colorBits
//
//	val minX = deferredIcon.bb.min.x + 1
//	val maxX = deferredIcon.bb.max.x - 1
//	val minY = deferredIcon.bb.min.y + 1
//	val maxY = deferredIcon.bb.max.y - 1
//
//	// Triangle 1
//	indices[indiceIdx++] = 1.toShort()
//	indices[indiceIdx++] = 0.toShort()
//	indices[indiceIdx++] = 2.toShort()
//
//	// Triangle 2
//	indices[indiceIdx++] = 0.toShort()
//	indices[indiceIdx++] = 3.toShort()
//	indices[indiceIdx++] = 2.toShort()
//
//	if (centerTex != null) {
//		vertex(minX, minY, colorBits, baseTex.u, baseTex.v2, centerTex.u - centerXOffset, centerTex.v2 + centerYOffset);
//		vertex(maxX, minY, colorBits, baseTex.u2, baseTex.v2, centerTex.u2 + centerXOffset, centerTex.v2 + centerYOffset);
//		vertex(maxX, maxY, colorBits, baseTex.u2, baseTex.v, centerTex.u2 + centerXOffset, centerTex.v - centerYOffset);
//		vertex(minX, maxY, colorBits, baseTex.u, baseTex.v, centerTex.u - centerXOffset, centerTex.v - centerYOffset);
//	} else {
//		vertex(minX, minY, colorBits, baseTex.u, baseTex.v2, 1f, 1f);
//		vertex(maxX, minY, colorBits, baseTex.u2, baseTex.v2, 1f, 1f);
//		vertex(maxX, maxY, colorBits, baseTex.u2, baseTex.v, 1f, 1f);
//		vertex(minX, maxY, colorBits, baseTex.u, baseTex.v, 1f, 1f);
//	}
//
//	mesh.setVertices(vertices, 0, vertexIdx)
//	mesh.setIndices(indices, 0, indiceIdx)
//	mesh.render(iconShader, GL20.GL_TRIANGLES)
//	
//	Gdx.gl.glUseProgram(lastProgram);
//	Gdx.gl.glBindBuffer(GL20.GL_ARRAY_BUFFER, lastArrayBuffer)
//	Gdx.gl.glBindBuffer(GL20.GL_ELEMENT_ARRAY_BUFFER, lastElementBuffer)
//	Gdx.gl30.glBindVertexArray(lastVertexArray)
}

void EmpireOverviewWindow::postDraw() {

	if (deferredIcons.size() == 0) {
		return;
	}
	
//	val projectionMatrix = imguiCamera.combined
//	
//	val texture = renderSystemGlobalData.strategicIconTexture
//	val vertices = renderSystemGlobalData.strategicIconVertices
//	val indices = renderSystemGlobalData.strategicIconIndices
//	val iconShader = renderSystemGlobalData.strategicIconShader
//	val mesh = renderSystemWindowData.strategicIconMesh
//	
//	iconShader.bind()
//	iconShader.setUniformMatrix("u_projTrans", projectionMatrix);
//	iconShader.setUniformi("u_texture", 14);
//	
//	Gdx.gl.glActiveTexture(GL20.GL_TEXTURE14)
//	texture.bind()
//	Gdx.gl.glActiveTexture(GL20.GL_TEXTURE0)
//	
//	Gdx.gl.glEnable(GL30.GL_BLEND);
//	Gdx.gl.glBlendFunc(GL30.GL_SRC_ALPHA, GL30.GL_ONE_MINUS_SRC_ALPHA);
//	
//	var vertexIdx = 0
//	var indiceIdx = 0
//	var stride = 0
//	
//	// Offset center texCoords by diff in texture sizes. 7 15 3+1+3 7+1+7 7-3=4
//	val centerXOffset = 4f / texture.width
//	val centerYOffset = 4f / texture.height
//	
//	fun vertex(x: Float, y: Float, colorBits: Float, baseU: Float, baseV: Float, centerU: Float, centerV: Float) {
//		vertices[vertexIdx++] = x;
//		vertices[vertexIdx++] = y;
//		vertices[vertexIdx++] = colorBits
//		vertices[vertexIdx++] = baseU;
//		vertices[vertexIdx++] = baseV;
//		vertices[vertexIdx++] = centerU;
//		vertices[vertexIdx++] = centerV;
//	}
//	
//	deferredIcons.forEachFast { deferredIcon ->
//		
//		val baseTex = deferredIcon.icon!!.baseTexture
//		val centerTex = deferredIcon.icon!!.centerTexture
//		val colorBits = deferredIcon.colorBits
//		
//		val minX = deferredIcon.bb.min.x + 1
//		val maxX = deferredIcon.bb.max.x - 1
//		val minY = deferredIcon.bb.min.y + 1
//		val maxY = deferredIcon.bb.max.y - 1
//		
//		// Triangle 1
//		indices[indiceIdx++] = (stride + 1).toShort()
//		indices[indiceIdx++] = (stride + 0).toShort()
//		indices[indiceIdx++] = (stride + 2).toShort()
//		
//		// Triangle 2
//		indices[indiceIdx++] = (stride + 0).toShort()
//		indices[indiceIdx++] = (stride + 3).toShort()
//		indices[indiceIdx++] = (stride + 2).toShort()
//		
//		stride += 4
//		
//		if (centerTex != null) {
//			vertex(minX, minY, colorBits, baseTex.u, baseTex.v2, centerTex.u - centerXOffset, centerTex.v2 + centerYOffset);
//			vertex(maxX, minY, colorBits, baseTex.u2, baseTex.v2, centerTex.u2 + centerXOffset, centerTex.v2 + centerYOffset);
//			vertex(maxX, maxY, colorBits, baseTex.u2, baseTex.v, centerTex.u2 + centerXOffset, centerTex.v - centerYOffset);
//			vertex(minX, maxY, colorBits, baseTex.u, baseTex.v, centerTex.u - centerXOffset, centerTex.v - centerYOffset);
//		} else {
//			vertex(minX, minY, colorBits, baseTex.u, baseTex.v2, 1f, 1f);
//			vertex(maxX, minY, colorBits, baseTex.u2, baseTex.v2, 1f, 1f);
//			vertex(maxX, maxY, colorBits, baseTex.u2, baseTex.v, 1f, 1f);
//			vertex(minX, maxY, colorBits, baseTex.u, baseTex.v, 1f, 1f);
//		}
//		
//		if (indiceIdx >= mesh.maxIndices) {
//			mesh.setVertices(vertices, 0, vertexIdx)
//			mesh.setIndices(indices, 0, indiceIdx)
//			mesh.render(iconShader, GL20.GL_TRIANGLES)
//			
//			vertexIdx = 0
//			indiceIdx = 0
//			stride = 0
//		}
//		
//		pool.free(deferredIcon)
//	}
	
	deferredIcons.clear();
	
//	if (indiceIdx > 0) {
//		mesh.setVertices(vertices, 0, vertexIdx)
//		mesh.setIndices(indices, 0, indiceIdx)
//		mesh.render(iconShader, GL20.GL_TRIANGLES)
//	}
//	
//	Gdx.gl.glDisable(GL30.GL_BLEND);
}
