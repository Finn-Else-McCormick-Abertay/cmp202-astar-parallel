#include "PathfindingSettings.h"

#include "../Singleton.h"
#include "../../imgui/imgui.h"

PathfindingSettings::PathfindingSettings() {
	m_algorithms.emplace_back(aStarSinglethreaded<Vec2, float>, "A* Single Threaded");

	m_heuristics.emplace_back(
		[](const Vec2& val, const Vec2& goalVal) { return (val - goalVal).length(); },
		"Straight Line Distance");
	m_heuristics.emplace_back(
		[](const Vec2& val, const Vec2& goalVal) { return abs(val.x - goalVal.x) + abs(val.y - goalVal.y); },
		"Manhattan Distance"
	);
}

bool PathfindingSettings::findPath() {
	Singleton::path() = m_algorithms[m_algorithmIndex].first(Singleton::graph(), m_startIndex, m_goalIndex, m_heuristics[m_heuristicIndex].first);
	return Singleton::path().size() > 0;
}

void PathfindingSettings::addMenuBarItem() {
	if (ImGui::BeginMenu("Pathfinding")) {
		if (ImGui::MenuItem("Settings")) {
			m_show = true;
			ImGui::SetWindowFocus("Pathfinding Settings");
		}
		ImGui::EndMenu();
	}
}

void PathfindingSettings::imguiDrawWindow(int width, int height) {
	if (!m_show) { return; }

	float popupWidth = 300, popupHeight = 130;
	ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
	ImGui::Begin("Pathfinding Settings", &m_show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	ImGui::Text("Algorithm");
	if (ImGui::BeginCombo("##algorithmCombo", m_algorithms[m_algorithmIndex].second.c_str())) {
		for (int n = 0; n < m_algorithms.size(); n++)
		{
			bool is_selected = (m_algorithmIndex == n);
			if (ImGui::Selectable(m_algorithms[m_algorithmIndex].second.c_str(), is_selected)) {
				m_algorithmIndex = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::Text("Heuristic");
	if (ImGui::BeginCombo("##heuristicCombo", m_heuristics[m_heuristicIndex].second.c_str())) {
		for (int n = 0; n < m_heuristics.size(); n++)
		{
			bool is_selected = (m_heuristicIndex == n);
			if (ImGui::Selectable(m_heuristics[n].second.c_str(), is_selected)) {
				m_heuristicIndex = n;
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void PathfindingSettings::imguiDrawControlGroup() {
	ImGui::BeginGroup();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Start", &m_startIndex)) {
		if (Singleton::graph().has(m_startIndex)) {
			m_groupMessage.first = "";
		}
		else {
			m_groupMessage.first = "Invalid index";
			m_groupMessage.second = true;
		}
	}
	ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Goal", &m_goalIndex)) {
		if (Singleton::graph().has(m_goalIndex)) {
			m_groupMessage.first = "";
		}
		else {
			m_groupMessage.first = "Invalid index";
			m_groupMessage.second = true;
		}
	}
	ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
	if (ImGui::Button("Find Path", ImVec2(100, 20))) {
		if (Singleton::graph().has(m_startIndex) && Singleton::graph().has(m_goalIndex)) {
			if (m_startIndex != m_goalIndex) {
				if (findPath()) {
					m_groupMessage.first = "";
				}
				else {
					m_groupMessage.first = "Path could not be found";
					m_groupMessage.second = true;
				}
			}
			else {
				m_groupMessage.first = "Start cannot be goal";
				m_groupMessage.second = true;
			}
		}
		else {
			m_groupMessage.first = "Invalid index";
			m_groupMessage.second = true;
		}
	}
	ImGui::SameLine();
	ImColor textColor = m_groupMessage.second ? ImColor(1.f, 0.1f, 0.1f) : ImColor(0.f, 0.8f, 0.2f);
	ImGui::TextColored(textColor, " %s", m_groupMessage.first.c_str());
	ImGui::EndGroup();
}