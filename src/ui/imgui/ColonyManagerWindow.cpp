#include <map>
#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include "Aurora.hpp"
#include "ColonyManagerWindow.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/Utils.hpp"
#include "galaxy/Player.hpp"
#include "galaxy/Empire.hpp"

#include "starsystems/components/SimpleComponents.hpp"
#include "starsystems/components/ColonyComponents.hpp"
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
			fmt::format_to(std::back_inserter(strBuf), "Colonies {}", empire.colonies.size());
			if (ImGui::CollapsingHeader(strBuf.data(), ImGuiTreeNodeFlags_DefaultOpen)) {
				
				std::map<StarSystem*, std::vector<EntityReference*>, std::function<decltype(StarSystemCompare)>>
				 systemColonyMap(StarSystemCompare);
				
				for (EntityReference& colonyRef : empire.colonies) {
					std::vector<EntityReference*>& colonies = systemColonyMap[colonyRef.system];
					colonies.push_back(&colonyRef);
				}
				
				for (auto& [starSystem, colonies] : systemColonyMap) {
					strBuf.clear();
					fmt::format_to(std::back_inserter(strBuf), "{}", starSystem->galacticEntityID);
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
							
							fmt::format_to(std::back_inserter(strBuf), "{}", entityID);
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
		}
		ImGui::EndChild();
	}
	ImGui::End();
};
