#include <map>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include "Aurora.hpp"
#include "ColonyManagerWindow.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"
#include "utils/ImGuiUtils.hpp"
#include "utils/Format.hpp"
#include "galaxy/Galaxy.hpp"
#include "galaxy/Player.hpp"
#include "galaxy/Empire.hpp"
#include "starsystems/systems/Systems.hpp"
#include "starsystems/components/SimpleComponents.hpp"
#include "starsystems/components/ColonyComponents.hpp"
#include "starsystems/components/CargoComponent.hpp"
#include "starsystems/components/HealthComponents.hpp"

static bool StarSystemCompare(const StarSystem* s1, const StarSystem* s2) {
	return *s1 < *s2;
}

void ColonyManagerWindow::render() {
	if (ImGui::Begin("Colony Manager", &visible, ImGuiWindowFlags_MenuBar)) {
		
		Empire& empire = *Player::current->empire;
		
		if (!selectedColony() && !empire.colonies.empty()) {
			selectedColony = empire.colonies[0];
		}
		
		auto strBuf = fmt::memory_buffer();
		
		if (ImGui::BeginChild("Colonies", ImVec2(200, 0), true, ImGuiWindowFlags_None)) {
			strBuf.clear();
			fmt::format_to(std::back_inserter(strBuf), "Colonies {}{}", empire.colonies.size(), '\0');
			if (ImGui::CollapsingHeader(strBuf.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
				
				std::map<StarSystem*, std::vector<EntityReference*>, std::function<decltype(StarSystemCompare)>>
				 systemColonyMap(StarSystemCompare);
				
				for (EntityReference& colonyRef : empire.colonies) {
					std::vector<EntityReference*>& colonies = systemColonyMap[colonyRef.system];
					colonies.push_back(&colonyRef);
				}
				
				for (auto& [starSystem, colonies] : systemColonyMap) {
					strBuf.clear();
					fmt::format_to(std::back_inserter(strBuf), "{}{}", starSystem->galacticEntityID, '\0');
					if (ImGui::TreeNodeEx(strBuf.data(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen, "System %s", starSystem->name.data())) {
						
						EntityReference* newSelectedColony = nullptr;
						
						for (EntityReference* colonyRef : colonies) {
							
							entt::entity entityID = colonyRef->entityID;
							
							NameComponent& nameComponent = starSystem->registry.get<NameComponent>(entityID);
							ColonyComponent& colonyComponent = starSystem->registry.get<ColonyComponent>(entityID);
							
							auto nodeFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NoTreePushOnOpen;
							
							if (*colonyRef == selectedColony) {
								nodeFlags |= ImGuiTreeNodeFlags_Selected;
							}
							
							strBuf.clear();
							fmt::format_to(std::back_inserter(strBuf), "{}{}", entityID, '\0');
							ImGui::TreeNodeEx(strBuf.data(), nodeFlags, "%s - %lu", nameComponent.name.data(), colonyComponent.population);
							
							if (ImGui::IsItemClicked()) {
								newSelectedColony = colonyRef;
							}
						}
						
						if (newSelectedColony != nullptr) {
							selectedColony = *newSelectedColony;
						}
					}
				}
			}
			
			if (ImGui::CollapsingHeader("Stations 0", ImGuiTreeNodeFlags_DefaultOpen)) {
				
			}
		}
		ImGui::EndChild();
		
		ImGui::SameLine();
		
		if (ImGui::BeginChild("Tabs", ImVec2(0, 0), false, ImGuiWindowFlags_None)) {
			if (!selectedColony()) {
				ImGui::TextUnformatted("No colony selected");
				
			} else if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown)) {
				
				StarSystem& system = *selectedColony.system;
				entt::entity entityID = selectedColony.entityID;
				
				ColonyComponent& colony = system.registry.get<ColonyComponent>(entityID);
				CargoComponent& cargo = system.registry.get<CargoComponent>(entityID);
				ColonySystem& colonySystem = *system.systems->colonySystem;
				
				if (ImGui::BeginTabItem("Shipyards")) {
					
					if (ImGui::BeginChild("List", ImVec2(0, -200), true, ImGuiWindowFlags_None)) {
						if (ImGui::BeginTable("table", 8, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings)) {
							ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoReorder);
							ImGui::TableSetupColumn("Type",	ImGuiTableColumnFlags_None);
							ImGui::TableSetupColumn("Capacity",	ImGuiTableColumnFlags_None);
							ImGui::TableSetupColumn("Tooled Hull",	ImGuiTableColumnFlags_None);
							ImGui::TableSetupColumn("Activity",	ImGuiTableColumnFlags_None);
							ImGui::TableSetupColumn("Progress",	ImGuiTableColumnFlags_None);
							ImGui::TableSetupColumn("Remaining",	ImGuiTableColumnFlags_None);
							ImGui::TableSetupColumn("Completion",	ImGuiTableColumnFlags_None);
							ImGui::TableHeadersRow();
							
							for (Shipyard& shipyard : colony.shipyards) {
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								
								bool shipyardSelected = &shipyard == selectedShipyard && selectedSlipway == nullptr;
								
								ImGuiStorage* storage = ImGui::GetCurrentWindow()->DC.StateStorage;
								
								strBuf.clear();
								fmt::format_to(std::back_inserter(strBuf), "##{}{}", (void*) &shipyard, '\0');
								char* buttonIDString = strBuf.data();
								ImGuiID buttonID = ImGui::GetID(buttonIDString, strBuf.data() + strBuf.size());
								bool shipyardOpen = storage->GetBool(buttonID, false);
								float fontSize = ImGui::GetCurrentContext()->FontSize;
								
								if (ImGui::ArrowButtonEx(buttonIDString, shipyardOpen ? ImGuiDir_Down : ImGuiDir_Right, ImVec2(fontSize, fontSize), ImGuiButtonFlags_None)) {
									storage->SetBool(buttonID, !shipyardOpen);
								}
								
								ImGui::TableNextColumn();
								
//								ImGui::SameLine();
								strBuf.clear();
								fmt::format_to(std::back_inserter(strBuf), "{} - {}{}", shipyard.type->abbreviation, shipyard.location->abbreviation, '\0');
								if (ImGui::Selectable(strBuf.data(), shipyardSelected, ImGuiSelectableFlags_SpanAllColumns)) {
									selectedShipyard = &shipyard;
									selectedSlipway = nullptr;
								}
								
								ImGui::TableNextColumn();
								rightAlignedTableText(volumeToString(shipyard.capacity).data());
								
								ImGui::TableNextColumn();
								if (shipyard.tooledHull != nullptr) {
									ImGui::Text("%s", shipyard.tooledHull->toString().data());
								} else {
									ImGui::TextUnformatted("None");
								}
								
								ImGui::TableNextColumn();
								const ShipyardModification* modActivity = shipyard.modificationActivity;
								if (modActivity != nullptr) {
									ImGui::Text("%s", modActivity->getDescription().data());
									ImGui::TableNextColumn();
									
									if (shipyard.modificationProgress == 0L) {
										ImGui::Selectable("0%");
									} else {
										float progress = (100 * shipyard.modificationProgress) / modActivity->getCost(shipyard);
										strBuf.clear();
										fmt::format_to(std::back_inserter(strBuf), "{}%{}", progress, '\0');
										ImGui::Selectable(strBuf.data());
									}
									
									ImGui::TableNextColumn();
									uint32_t daysToCompletion = (modActivity->getCost(shipyard) - shipyard.modificationProgress) / (24 * shipyard.modificationRate);
									
									rightAlignedTableText(daysToRemaining(daysToCompletion).data());
									ImGui::TableNextColumn();
									ImGui::TextUnformatted(daysToDate(Aurora.galaxy->day + daysToCompletion).data());
									
								} else {
									ImGui::TextUnformatted("");
									ImGui::TableNextColumn();
									ImGui::TextUnformatted("");
									ImGui::TableNextColumn();
									ImGui::TextUnformatted("");
									ImGui::TableNextColumn();
									ImGui::TextUnformatted("");
									ImGui::TableNextColumn();
								}
								
								if (shipyardOpen) {
									for (ShipyardSlipway& slipway : shipyard.slipways) {
										ImGui::TableNextRow();
										ImGui::TableNextColumn();
										
										const ShipHull* hull = slipway.hull;
										bool slipwaySelected = &slipway == selectedSlipway;
										
										strBuf.clear();
										fmt::format_to(std::back_inserter(strBuf), "##{}{}", (void*) &slipway, '\0');
										if (ImGui::Selectable(strBuf.data(), slipwaySelected, ImGuiSelectableFlags_SpanAllColumns)) {
											selectedShipyard = &shipyard;
											selectedSlipway = &slipway;
										}
										
										ImGui::TableNextColumn();
										
										if (hull != nullptr) {
											rightAlignedTableText(massToString(hull->emptyMass).data());
											ImGui::TableNextColumn();
											rightAlignedTableText(volumeToString(hull->volume).data());
											ImGui::TableNextColumn();
											ImGui::TextUnformatted(hull->toString().data());
											ImGui::TableNextColumn();
											ImGui::TextUnformatted("Building");
											ImGui::TableNextColumn();
											ImGui::Text("%d%%", slipway.progress());
											ImGui::TableNextColumn();
											uint64_t toBuild = slipway.totalCost() - slipway.totalUsedResources();
											uint32_t daysToCompletion = toBuild / (24 * shipyard.buildRate);
											if (daysToCompletion > 0) {
												rightAlignedTableText(daysToRemaining(daysToCompletion).data());
												
											} else {
												uint64_t secondsToCompletion = colonySystem.getInterval() * ((toBuild + (shipyard.buildRate - 1)) / shipyard.buildRate);
												secondsToCompletion -= Aurora.galaxy->time % colonySystem.getInterval();
												rightAlignedTableText(secondsToString(secondsToCompletion).data());
											}
											ImGui::TableNextColumn();
											ImGui::TextUnformatted(daysToDate(Aurora.galaxy->day + daysToCompletion).data());
											
										} else {
											rightAlignedTableText("-");
											ImGui::TableNextColumn();
											rightAlignedTableText("-");
											ImGui::TableNextColumn();
											ImGui::TextUnformatted("-");
											ImGui::TableNextColumn();
											ImGui::TextUnformatted("None");
											ImGui::TableNextColumn();
											ImGui::TextUnformatted("-");
											ImGui::TableNextColumn();
											rightAlignedTableText("-");
											ImGui::TableNextColumn();
											ImGui::TextUnformatted("-");
										}
									}
								}
							}
							
							ImGui::EndTable();
						}
					}
					ImGui::EndChild();
					
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Industry")) {
					ImGui::TextUnformatted("Munitions:");
					with_Group {
//						for(auto hull : cargo.munitions) {
//							ImGui::TextUnformatted(hull.name);
//						}
					}
					
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Mining")) {
					
					ImGui::EndTabItem();
				}
				
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
};
