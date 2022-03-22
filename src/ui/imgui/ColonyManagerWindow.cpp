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
				
				PlanetComponent& planet = system.registry.get<PlanetComponent>(entityID);
				ColonyComponent& colony = system.registry.get<ColonyComponent>(entityID);
				CargoComponent& cargo = system.registry.get<CargoComponent>(entityID);
				ColonySystem& colonySystem = *system.systems->colonySystem;
				
				if (ImGui::BeginTabItem("Shipyards")) {
					
					if (ImGui::BeginChild("List", ImVec2(0, -200), true, ImGuiWindowFlags_None)) {
						if (ImGui::BeginTable("shipyards", 8, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings)) {
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
				
				static bool first = true;
				if (ImGui::BeginTabItem("Industry", nullptr, first ? ImGuiTabItemFlags_SetSelected : 0)) {
					first = false;
					
					uint8_t lastDistrictType = DistrictType::_size_constant;
					
					auto districtButton = [&](std::string_view symbol, const District& district){
						if (lastDistrictType != DistrictType::_size_constant) {
							if (district.type == lastDistrictType) {
								ImGui::SameLine(0, 2);
							} else {
								ImGui::SameLine(0, 10);
							}
						}
						lastDistrictType = district.type;
						
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
						ImGui::PushID(reinterpret_cast<intptr_t>(&district));
						
						if (ImGui::Button("##district", ImVec2(30, 32))) {
							colony.districtAmounts[DistrictPnt(&district)]++;
						}
						
						if (ImGui::IsItemVisible()) {
							uint16_t amount = colony.districtAmounts[DistrictPnt(&district)];
							
							const ImGuiStyle& style = ImGui::GetStyle();
							const ImVec2 min = ImGui::GetItemRectMin();
							const ImVec2 max = ImGui::GetItemRectMax();
							const ImRect bb(min, max);
							ImVec2 label_size = ImGui::CalcTextSize(symbol.cbegin(), symbol.cend(), true);
							ImGui::RenderTextClipped(min + style.FramePadding, max - style.FramePadding, symbol.cbegin(), symbol.cend(), &label_size, ImVec2(0.5f, 0), &bb);
							
							strBuf.clear();
							fmt::format_to(std::back_inserter(strBuf), "{}{}", amount, '\0');
							label_size = ImGui::CalcTextSize(strBuf.begin(), strBuf.end(), true);
							ImGui::RenderTextClipped(min + style.FramePadding, max - style.FramePadding, strBuf.begin(), strBuf.end(), &label_size, ImVec2(0.5f, 1.0f), &bb);
							
							if (ImGui::IsItemHovered()) {
								with_Tooltip {
									ImGui::TextUnformatted(district.name.cbegin(), district.name.cend());
									ImGui::Text("Type: %s", district.type._to_string());
									ImGui::Text("Amount: %u km²", amount);
								}
							}
						}
						
						ImGui::PopID();
						ImGui::PopStyleVar();
					};
					
					districtButton("Low", Districts::HousingLowDensity);
					districtButton("Hi", Districts::HousingHighDensity);
					
					districtButton("F", Districts::Farm);
					
					districtButton("Ind", Districts::GeneralIndustry);
					districtButton("BF", Districts::RefineryBlastFurnace);
					districtButton("AF", Districts::RefineryArcFurnace);
					districtButton("GS", Districts::RefinerySmeltery);
					districtButton("ScF", Districts::RefinerySemiconductorFab);
					districtButton("UE", Districts::RefineryEnricher);
					districtButton("CP", Districts::RefineryChemicalPlant);
					districtButton("FR", Districts::RefineryFuelRefinery);
					districtButton("Li", Districts::RefineryLithium);
					
					districtButton("Sol", Districts::PowerSolar);
					districtButton("Coa", Districts::PowerCoal);
					districtButton("Fis", Districts::PowerFission);
					districtButton("Fus", Districts::PowerFusion);
					
					districtButton("MS", Districts::MineSurface);
					districtButton("MC", Districts::MineCrust);
					districtButton("MM", Districts::MineMantle);
					districtButton("MMC", Districts::MineMoltenCore);
					
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Mining")) {
					
					for (uint_fast8_t layer = 0; layer < MiningLayer::_size_constant; layer++){
						SmallList<OreDeposit, 32>& deposits = planet.oreDeposits[layer];
						BitVector32& discovered = planet.discoveredOreDeposits[layer];
						
						ImGui::TextUnformatted(MiningLayer::_from_index(layer)._to_string());
						ImGui::SameLine(80);
						
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
						for (uint_fast8_t i = 0; i < deposits.size(); i++) {
							OreDeposit& deposit = deposits[i];
							
							if (i > 0) ImGui::SameLine();
							
							const char* text = discovered[i] ? deposit.type->symbol.data() : "?";
							ImGui::PushID(i + layer * 128);
							if (ImGui::Button(text, ImVec2(21, 20))) {
								discovered[i] = true;
								planet.minableResources[layer][deposit.type] += deposit.amount;
							}
							if (discovered[i] && ImGui::IsItemHovered()) {
								with_Tooltip {
									ImGui::TextUnformatted(deposit.type->name.cbegin(), deposit.type->name.cend());
									ImGui::Text("%lu", deposit.amount);
								}
							}
							ImGui::PopID();
						}
						ImGui::PopStyleVar();
					}
					
					ImGui::TextUnformatted("Discovered and remaining ores:");
					if (ImGui::BeginTable("discovered_ores", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBodyUntilResize)) {
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize);
						
						auto printMinableResources = [&](MiningLayer layer){
//							if (ImGui::TableGetRowIndex() > 0) {
//								ImGui::TableNextRow(0, 5);
//							}
							
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(layer._to_string());
							ImGui::SameLine(0, 0);
							ImGui::TextUnformatted(":");
							
							uint_fast8_t columns = 3;
							uint_fast8_t rows = (Resources::ALL_ORE_size + columns - 1) / columns;
							for (uint_fast8_t row = 0; row < rows; row++) {
								ImGui::TableNextRow();
								for (uint_fast8_t col = 0; col < columns; col++) {
									size_t idx = row + col * rows;
									if (idx >= Resources::ALL_ORE_size) break;
									
									const Resource* res = Resources::ALL_ORE[idx];
									ImGui::TableNextColumn();
									rightAlignedTableText("%6d", planet.minableResources[layer][idx]);
									ImGui::TableNextColumn();
									ImGui::TextUnformatted(res->name.cbegin(), res->name.cend());
								}
							}
						};
						
						printMinableResources(MiningLayer::Surface);
						printMinableResources(MiningLayer::Crust);
						printMinableResources(MiningLayer::Mantle);
						printMinableResources(MiningLayer::MoltenCore);
						
						ImGui::EndTable();
					}
					
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Resources")) {
					if (ImGui::BeginTable("resources", 6, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBodyUntilResize)) {
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						
						auto printResources = [&](std::string_view name, uint32_t columns, const Resource* const* cargoType, size_t size){
							if (ImGui::TableGetRowIndex() > 0) {
								ImGui::TableNextRow(0, 5);
							}
							
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(name.begin());
						
							uint_fast8_t rows = (size + columns - 1) / columns;
							for (uint_fast8_t row = 0; row < rows; row++) {
								ImGui::TableNextRow();
								for (uint_fast8_t col = 0; col < columns; col++) {
									size_t idx = row + col * rows;
									if (idx >= size) break;
									
									const Resource* res = cargoType[idx];
									ImGui::TableNextColumn();
									rightAlignedTableText("%6d", cargo.getCargoAmount(res));
									ImGui::TableNextColumn();
									ImGui::TextUnformatted(res->name.cbegin(), res->name.cend());
								}
							}
						};
						
						printResources("Ores:", 3, CargoTypes::ORE_, CargoTypes::ORE_size);
						printResources("Refined:", 3, CargoTypes::REFINED_, CargoTypes::REFINED_size);
						printResources("Goods:", 3, Resources::ALL_GOODS_UI, Resources::ALL_GOODS_UI_size);
						printResources("Fuel:", 3, Resources::ALL_FUEL_UI, Resources::ALL_FUEL_UI_size);
						printResources("Munitions:", 3, CargoTypes::AMMUNITION_, CargoTypes::AMMUNITION_size);
						
						ImGui::EndTable();
					}
					
					if (ImGui::BeginTable("munitions", 8, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_NoBordersInBodyUntilResize)) {
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_None);
						ImGui::TableSetupColumn("",	ImGuiTableColumnFlags_NoResize);
						
						ImGui::TableNextRow();
						uint_fast16_t i = 0;
						for (auto hull : cargo.munitions) {
							ImGui::TableNextColumn();
							
							rightAlignedTableText("%4d", hull.second);
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(hull.first->name.c_str());
							
							if (++i >= 4) {
								i = 0;
								ImGui::TableNextRow();
							}
						}
						
						ImGui::EndTable();
					}
					
					ImGui::EndTabItem();
				}
				
				ImGui::EndTabBar();
			}
		}
		ImGui::EndChild();
	}
	ImGui::End();
};
