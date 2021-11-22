#include <imgui.h>
#include <imgui_internal.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include "Aurora.hpp"
#include "ProfilerWindow.hpp"
#include "starsystems/ShadowStarSystem.hpp"
#include "ui/imgui/ImGuiLayer.hpp"
#include "utils/ImGuiUtils.hpp"
#include "utils/Utils.hpp"
#include "utils/Format.hpp"

static const float ZOOM_MIN = 0.001f;
static const float ZOOM_MAX = 0.05f;

void ProfilerWindow::render() {
	
	layer.getMainWindow().profiling = visible && Aurora.galaxy->speed > 0ns;
	StarSystem* starSystem = layer.getMainWindow().starSystem;
	
	if (ImGui::Begin("Profiler", &visible, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar)) {
		
		if (starSystem != oldStarSystem) {
			if (oldStarSystem != nullptr) {
				oldStarSystem->shadow->profiling = false;
			}
			starSystem->shadow->profiling = true;
			oldStarSystem = starSystem;
		}
		
		ImGui::SliderScalar("Zoom", ImGuiDataType_Float, &zoom, &ZOOM_MIN, &ZOOM_MAX, nullptr, 1.0f);
		ImGui::TextUnformatted("TODO time");
		
		float backupPaddingY = ImGui::GetStyle().FramePadding.y;
		
		float x = 0, y = 0;
		float maxY = 0;
		uint64_t timeOffset = 0;
		int64_t scroll = 0;
		auto strBuf = fmt::memory_buffer();
		
		auto eventBar = [&](int64_t start, int64_t end, const char* name) -> bool {
			float x1 = x + (start - scroll) * zoom;
			float width = (end - start) * zoom;
			
			if (x1 < ImGui::GetCurrentWindowRead()->Size.x || x1 + width >= 0) {
				strBuf.clear();
				fmt::format_to(std::back_inserter(strBuf), "event {}", start);
				
				ImGuiID id = ImGui::GetCurrentWindowRead()->GetID(strBuf.data(), strBuf.data() + strBuf.size());
				ImVec2 pos { x1, y };
				ImVec2 size { width, ImGui::GetCurrentContext()->FontSize + 1 };
				ImRect bb { pos, pos + size };
				ImVec2 labelSize = ImGui::CalcTextSize(name);
				
				if (!ImGui::ItemAdd(bb, id)) return false;
				bool hovered;
				ImGui::ButtonBehavior(bb, id, &hovered, nullptr);
				
				// Render
				ImU32 color;
				if (hovered) {
					color = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
				} else {
					color = ImGui::GetColorU32(toLinearRGB(ImVec4(0.3f, 0.5f, 0.3f, 1.0f)));
				}
				
				ImGui::RenderNavHighlight(bb, id);
				ImGui::RenderFrame(bb.Min, bb.Max, color, true, 1.0f);
				ImGui::RenderTextClipped(bb.Min + ImVec2(1.0f, -1.0f), bb.Max - 1, name, nullptr, &labelSize, ImVec2(0.0f, 0.5f), &bb);
				
				if (y + size.y > maxY) {
					maxY = y + size.y;
				}
				
				return hovered;
			}
			
			return false;
		};
		
		std::function<uint32_t(ProfilerEvents&, uint32_t)> drawNestedEvents = [&](ProfilerEvents& events, uint32_t idx) {
			const ProfilerEvent& startEvent = events[idx++];
			const ProfilerEvent* endEvent = &events[idx];
			
			while (endEvent->name[0] != '\0') {
				y += 15;
				idx = drawNestedEvents(events, idx);
				y -= 15;
				endEvent = &events[idx];
			}
			
			if (eventBar(startEvent.time.count() - timeOffset, endEvent->time.count() - timeOffset, startEvent.name)) {
				strBuf.clear();
				fmt::format_to(std::back_inserter(strBuf), "{} {:u}", startEvent.name, endEvent->time - startEvent.time);
				strBuf.push_back('\0');
				ImGui::SetTooltip(strBuf.data());
			}
			
			return idx + 1;
		};
		
		auto drawEvents = [&](ProfilerEvents& events) {
			size_t size = events.size();
			
			if (size > 0) {
				if (size % 2 == 0) {
					
					ImGui::GetStyle().FramePadding.y = 0;
					x = ImGui::GetCurrentWindowRead()->DC.CursorPos.x;
					y = ImGui::GetCurrentWindowRead()->DC.CursorPos.y;
					maxY = y;
					
					uint32_t i=0;
					while (i < size - 1) {
						i = drawNestedEvents(events, i);
					}
					
					ImGui::GetStyle().FramePadding.y = backupPaddingY;
					
					ImVec2 pos { x, y };
					ImVec2 itemSize { ImGui::GetCurrentWindowRead()->Size.x, std::max(maxY - y, 32.0f) };
					ImGui::ItemSize(itemSize);
					
					ImRect bb { pos, pos + itemSize };
					ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(toLinearRGB(ImVec4(0.5f, 0.5f, 0.5f, 0.1f))), true, 1.0f);
					
					strBuf.clear();
					fmt::format_to(std::back_inserter(strBuf), "events {}", (void*) &events);
					ImGuiID id = ImGui::GetCurrentWindowRead()->GetID(strBuf.data(), strBuf.data() + strBuf.size());
					
					if (ImGui::ItemAdd(bb, id)) {
//						ImGui::ButtonBehavior(bb, id, &hovered, &held);
					}
					
				} else {
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Event size is not even!");
				}
			}
		};
		
		auto mouseScroll = [&](float scroll) { // negative scroll == zoom out
			float oldZoom = zoom;
			float sensitivity = 50; // higher less sensitive
			float zoomRatio = sensitivity * (zoom - ZOOM_MIN) / ZOOM_MAX;
			
			// linear
			zoomRatio = std::clamp((zoomRatio + scroll) / sensitivity, 0.0f, 1.0f);
			zoom = std::clamp(ZOOM_MIN + (ZOOM_MAX - ZOOM_MIN) * zoomRatio, ZOOM_MIN, ZOOM_MAX);
			
			// power
//			zoomRatio = std::clamp((zoomRatio + scroll) / sensitivity, 0.0f, 1.0f);
//			zoomRatio = std::pow(zoomRatio, 1.1);
//			zoom = std::clamp(ZOOM_MAX * zoomRatio, ZOOM_MIN, ZOOM_MAX);
			
			float mouseX = ImGui::GetMousePos().x - ImGui::GetCurrentWindowRead()->Pos.x;
			renderScrollX += (int64_t)(mouseX / oldZoom - mouseX / zoom);
			systemsScrollX += (int64_t)(mouseX / oldZoom - mouseX / zoom);
		};
		
		if (ImGui::BeginChild("render", ImVec2(0, 100), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove)) {
			ImGui::GetCurrentWindow()->ScrollMax.y = 1;
			
			if (ImGui::IsWindowFocused()) {
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
					renderScrollX = oldRenderScrollX - (uint64_t)(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x / zoom);
				} else {
					oldRenderScrollX = renderScrollX;
				}
			}
			
			if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused()) {
				float scroll = ImGui::GetCurrentContext()->IO.MouseWheel;
				
				if (scroll != 0) {
					mouseScroll(scroll);
				}
			}
			
			ProfilerEvents& events = layer.getMainWindow().profilerEvents;
			
			if (events.size() > 0) {
				timeOffset = events[0].time.count();
				scroll = renderScrollX;
				
				strBuf.clear();
				fmt::format_to(std::back_inserter(strBuf), "Renderer {} events", events.size());
				ImGui::TextUnformatted(strBuf.data(), strBuf.data() + strBuf.size());
				drawEvents(events);
			}
		}
		ImGui::EndChild();
		
		if (ImGui::BeginChild("galaxy", ImVec2(0, 200 * Aurora.galaxy->systems.size()), false, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove)) {
			ImGui::GetCurrentWindow()->ScrollMax.y = 1;
			
			if (ImGui::IsWindowFocused()) {
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
					systemsScrollX = oldSystemsScrollX - (uint64_t)(ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).x / zoom);
				} else {
					oldSystemsScrollX = systemsScrollX;
				}
			}
			
			if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused()) {
				float scroll = ImGui::GetCurrentContext()->IO.MouseWheel;
				
				if (scroll != 0) {
					mouseScroll(scroll);
				}
			}
			
			ProfilerEvents& galaxyEvents = Aurora.galaxy->shadow->profilerEvents;
			
			strBuf.clear();
			fmt::format_to(std::back_inserter(strBuf), "Galaxy {} events", galaxyEvents.size());
			ImGui::TextUnformatted(strBuf.data(), strBuf.data() + strBuf.size());
			
			if (galaxyEvents.size() > 0) {
				timeOffset = galaxyEvents[0].time.count();
				scroll = systemsScrollX;
				
				drawEvents(galaxyEvents);
				
				uint8_t i = 0;
				for (const StarSystem* system : Aurora.galaxy->systems) {
					ProfilerEvents& events = system->shadow->profilerEvents;
					
					strBuf.clear();
					// system->registry.entity(system->galacticEntityID)
					fmt::format_to(std::back_inserter(strBuf), "Star System {} - {} events", i++, events.size());
					ImGui::TextUnformatted(strBuf.data(), strBuf.data() + strBuf.size());
					
					if (events.size() > 0) {
						drawEvents(events);
					}
				}
			}
		}
		ImGui::EndChild();
		
	} else {
		starSystem->shadow->profiling = false;
	}
	
	ImGui::End();
};
