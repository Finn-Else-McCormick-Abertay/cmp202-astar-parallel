#include "PathfindingSettings.h"

#include "../Singleton.h"
#include "../../imgui/imgui.h"

#include "../Pathfinding/AStar.h"
#include "../Pathfinding/HDAStar.h"
#include "../Pathfinding/Heuristics.h"

#include "../Pathfinding/PathStreamOp.h"

PathfindingSettings::PathfindingSettings() {
	m_algorithms.emplace_back(aStarSequential<Vec2, float>, "A* Sequential");
	m_algorithms.emplace_back(hashDistributedAStarSharedMemory<Vec2, float>, "HDA* Parallel Shared Memory");

	m_heuristics.emplace_back(straightLineDistance, "Straight Line Distance");
	m_heuristics.emplace_back(manhattanDistance, "Manhattan Distance");
}

const PathfindingAlgorithm<Vec2, float>& PathfindingSettings::getCurrentAlgorithm() const { return m_algorithms[m_algorithmIndex].first; }
const Heuristic<Vec2, float>& PathfindingSettings::getCurrentHeuristic() const { return m_heuristics[m_heuristicIndex].first; }

bool PathfindingSettings::findPath() {
	Singleton::path() = getCurrentAlgorithm()(Singleton::graph(), m_startIndex, m_goalIndex, getCurrentHeuristic());
	if (Singleton::path().size() > 0) {
		Singleton::consoleOutput("Found path of length ", Singleton::path().size(), " from node ", m_startIndex, " to node ", m_goalIndex,
			" using ", m_algorithms.at(m_algorithmIndex).second, " with heuristic ", m_heuristics.at(m_heuristicIndex).second, ".");
		pathOutput(Singleton::getConsole(), Singleton::path(), Singleton::graph(), "\n");
		Singleton::consoleOutput();
		return true;
	}
	else {
		Singleton::consoleOutput("No path could be found between nodes ", m_startIndex, " and ", m_goalIndex,
			" using ", m_algorithms.at(m_algorithmIndex).second, " with heuristic ", m_heuristics.at(m_heuristicIndex).second, ".");
		return false;
	}
}

void PathfindingSettings::startProfiling() {
	m_profilerMessage.clear();
	if (m_profilerBlocking) {
		m_profiler = std::make_unique<ProfilerBlocking>(m_profilerIterations);
		Singleton::consoleOutput("Beginning blocking profiling session with ", m_profilerIterations, " iterations.");
		Singleton::consoleOutput("Algorithm: ", m_algorithms.at(m_algorithmIndex).second);
		Singleton::consoleOutput("Heuristic: ", m_heuristics.at(m_heuristicIndex).second);
		((ProfilerBlocking*)m_profiler.get())->performProfiling(getCurrentAlgorithm(), Singleton::graph(), m_startIndex, m_goalIndex, getCurrentHeuristic());
		finalProfilerMessage();
	}
	else {
		m_profiler = std::make_unique<ProfilerNonBlocking>(m_profilerIterations);
		Singleton::currentlyProfiling() = true;
		Singleton::consoleOutput("Beginning non-blocking profiling session with ", m_profilerIterations, " iterations.");
		Singleton::consoleOutput("Algorithm: ", m_algorithms.at(m_algorithmIndex).second);
		Singleton::consoleOutput("Heuristic: ", m_heuristics.at(m_heuristicIndex).second);
		((ProfilerNonBlocking*)m_profiler.get())->startProfiling(getCurrentAlgorithm(), std::cref(Singleton::graph()), m_startIndex, m_goalIndex, getCurrentHeuristic());
	}
}

void PathfindingSettings::finalProfilerMessage() {
	m_profilerMessage.setMessage("Profiling complete.");
	Singleton::consoleOutput("Profiling complete.");
	Singleton::consoleOutput("Results:");
	auto timeStats = m_profiler->timingResults();
	for (auto& time : timeStats.times()) {
		Singleton::consoleOutput(time.asSecondsFull(), " seconds / ", time);
	}
	Singleton::consoleOutput();
	Singleton::consoleOutput("Median: ", timeStats.median());
	Singleton::consoleOutput("Mean: ", timeStats.mean());
	Singleton::consoleOutput("Standard Deviation: ", timeStats.standardDeviation());
}

void PathfindingSettings::checkOnProfiling() {
	if (!Singleton::currentlyProfiling() || !m_profiler || m_profilerBlocking) { return; }

	if (((ProfilerNonBlocking*)m_profiler.get())->isFinished()) {
		Singleton::currentlyProfiling() = false;
		finalProfilerMessage();
	}
}


void PathfindingSettings::setAlgorithm(int v) { m_algorithmIndex = v; }
void PathfindingSettings::setHeuristic(int v) { m_heuristicIndex = v; }

void PathfindingSettings::setIndices(int start, int goal) { m_startIndex = start; m_goalIndex = goal; }

void PathfindingSettings::addMenuBarItem() {
	if (ImGui::BeginMenu("Pathfinding")) {
		if (ImGui::MenuItem("Settings")) {
			m_showSettingsDialog = true;
			ImGui::SetWindowFocus("Pathfinding Settings");
		}
		if (ImGui::MenuItem("Profiling")) {
			m_showProfilingDialog = true;
			ImGui::SetWindowFocus("Profiling");
		}
		ImGui::EndMenu();
	}
}

void PathfindingSettings::imguiDrawWindow(int width, int height) {
	checkOnProfiling();
	bool disabled = Singleton::currentlyProfiling();

	if (m_showSettingsDialog) {
		float popupWidth = 300, popupHeight = 140;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Pathfinding Settings", &m_showSettingsDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (disabled) { ImGui::BeginDisabled(); }

		float comboWidth = 280;
		ImGui::Text("Algorithm");
		ImGui::SetNextItemWidth(comboWidth);
		if (ImGui::BeginCombo("##algorithmCombo", m_algorithms[m_algorithmIndex].second.c_str())) {
			for (int n = 0; n < m_algorithms.size(); n++)
			{
				bool is_selected = (m_algorithmIndex == n);
				if (ImGui::Selectable(m_algorithms[n].second.c_str(), is_selected)) {
					m_algorithmIndex = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("Heuristic");
		ImGui::SetNextItemWidth(comboWidth);
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
		if (m_algorithmIndex == 0) { ImGui::BeginDisabled(); }
		ImGui::InputInt("Threads", &g_numThreads);
		if (m_algorithmIndex == 0) { ImGui::EndDisabled(); }

		if (disabled) { ImGui::EndDisabled(); }
		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (m_showProfilingDialog) {
		float popupWidth = 300, popupHeight = 140;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Profiling", &m_showProfilingDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (disabled) { ImGui::BeginDisabled(); }

		ImGui::SetNextItemWidth(100);
		ImGui::InputInt("Number of iterations", &m_profilerIterations, 100, 1000);
		ImGui::Checkbox("Blocking", &m_profilerBlocking);
		ImGui::SetItemTooltip("Whether to launch from a detached thread.\n(Blocking is potentially more accurate but causes window to freeze.)");
		if (ImGui::Button("Begin", ImVec2(100, 20))) {
			startProfiling();
		}
		m_profilerMessage.draw();

		if (disabled) { ImGui::EndDisabled(); }
		ImGui::End();
		ImGui::PopStyleVar();
	}
}

void PathfindingSettings::imguiDrawControlGroup() {
	ImGui::BeginGroup();
	if (Singleton::currentlyProfiling()) { ImGui::BeginDisabled(); }

	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Start", &m_startIndex)) {
		if (Singleton::graph().has(m_startIndex)) { m_groupMessage.clear(); }
		else {
			m_groupMessage.setMessage("Invalid index", true);
		}
	}
	ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	if (ImGui::InputInt("Goal", &m_goalIndex)) {
		if (Singleton::graph().has(m_goalIndex)) { m_groupMessage.clear(); }
		else {
			m_groupMessage.setMessage("Invalid index", true);
		}
	}
	ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
	if (ImGui::Button("Find Path", ImVec2(100, 20))) {
		if (Singleton::graph().has(m_startIndex) && Singleton::graph().has(m_goalIndex)) {
			if (m_startIndex != m_goalIndex) {
				if (findPath()) { m_groupMessage.clear(); }
				else {
					m_groupMessage.setMessage("Path could not be found", true);
				}
			}
			else { m_groupMessage.setMessage("Start cannot be goal", true); }
		}
		else { m_groupMessage.setMessage("Invalid index", true); }
	}
	ImGui::SameLine(); m_groupMessage.draw();

	if (Singleton::currentlyProfiling()) { ImGui::EndDisabled(); }
	ImGui::EndGroup();
}