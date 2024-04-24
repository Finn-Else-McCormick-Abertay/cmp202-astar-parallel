#include "GraphEdit.h"
#include "../../imgui/imgui.h"

#include "../Singleton.h"
#include <string>

#include "../Graph/GraphJSON.h"

#include "../Graph/GraphDisplay.h"
#include "../Graph/GenerateGraph.h"

#include "../Pathfinding/Heuristics.h"

GraphEdit::GraphEdit() {
	m_heuristics.emplace_back(straightLineDistance, "Straight Line Distance");
	m_heuristics.emplace_back(manhattanDistance, "Manhattan Distance");
}

void GraphEdit::saveGraph(std::string path) {
	auto resultingPath = saveToFile(Singleton::graph(), path);
	m_saveLoadMessage.setMessage("Saved to " + resultingPath.generic_string());
	Singleton::consoleOutput("Saved graph to file at local path ", resultingPath);
}

void GraphEdit::loadGraph(std::string path) {
	auto [loadedGraph, resultingPath] = loadFromFile<Vec2, float>(path);
	if (loadedGraph.size() != 0) {
		Singleton::graph() = loadedGraph;
		Singleton::path() = Path();
		m_saveLoadMessage.setMessage("Loaded from " + resultingPath.generic_string());
		Singleton::consoleOutput("Loaded graph from file at local path ", resultingPath);
	}
	else { m_saveLoadMessage.setMessage("No such file " + resultingPath.generic_string(), true); }
}

void GraphEdit::generateGraph() {
	Singleton::graph() = GenerateKNearest(m_generate_numNodes, m_generate_k,
		Vec2(m_generate_lowerBound[0], m_generate_lowerBound[1]), Vec2(m_generate_upperBound[0], m_generate_upperBound[1]),
		m_heuristics[m_heuristicIndex].first, m_generate_doubleEdged);
	Singleton::recalculateEdgeWeights();
	Singleton::path() = Path();
	Singleton::consoleOutput("Generated ", (m_generate_doubleEdged ? " double-edged " : ""), "k-nearest graph with size=",
		m_generate_numNodes, " and k=", m_generate_k, " using heuristic ", m_heuristics.at(m_heuristicIndex).second, ".");
}

void GraphEdit::addMenuBarItem() {
	if (ImGui::BeginMenu("Graph")) {
		if (ImGui::MenuItem("Edit")) {
			m_show = true;
			ImGui::SetWindowFocus("Edit Graph");
		}
		if (ImGui::MenuItem("Generate")) {
			m_showGenerateDialog = true;
			ImGui::SetWindowFocus("Generate Graph");
		}
		if (ImGui::MenuItem("Save")) {
			if (!m_saveLoadDialogIsSave) { m_saveLoadMessage.clear(); }
			m_showSaveLoadDialog = true;
			m_saveLoadDialogIsSave = true;
			ImGui::SetWindowFocus("Save");
		}
		if (ImGui::MenuItem("Load")) {
			if (m_saveLoadDialogIsSave) { m_saveLoadMessage.clear(); }
			m_showSaveLoadDialog = true;
			m_saveLoadDialogIsSave = false;
			ImGui::SetWindowFocus("Load");
		}
		ImGui::EndMenu();
	}
}

void GraphEdit::imguiDrawWindow(int width, int height) {
	auto& graph = Singleton::graph();

	if (m_showSaveLoadDialog) {
		float popupWidth = 200, popupHeight = 100;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		const char* label = (m_saveLoadDialogIsSave) ? "Save" : "Load";
		ImGui::Begin(label, &m_showSaveLoadDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (Singleton::currentlyProfiling()) { ImGui::BeginDisabled(); }

		auto buttonOrEnterPressed = [&]() {
			if (m_saveLoadDialogIsSave) { saveGraph(std::string(m_inputSavePath)); }
			else { loadGraph(std::string(m_inputSavePath)); }
			};

		ImGui::SetNextItemWidth(180);
		if (ImGui::InputText("##filepathInput", m_inputSavePath, IM_ARRAYSIZE(m_inputSavePath), ImGuiInputTextFlags_EnterReturnsTrue)) {
			buttonOrEnterPressed();
		}
		if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0) && !Singleton::currentlyProfiling()) { ImGui::SetKeyboardFocusHere(-1); }
		ImGui::PushID("##saveloadbutton");
		if (ImGui::Button(label, ImVec2(100, 20))) {
			buttonOrEnterPressed();
		}
		ImGui::PopID();
		m_saveLoadMessage.draw();

		if (Singleton::currentlyProfiling()) { ImGui::EndDisabled(); }
		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (m_showGenerateDialog) {
		float popupWidth = 350, popupHeight = 195;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Generate Graph", &m_showGenerateDialog, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (Singleton::currentlyProfiling()) { ImGui::BeginDisabled(); }
		float comboWidth = 330;

		ImGui::InputInt("Number of nodes", &m_generate_numNodes);
		ImGui::InputInt("Edges per node", &m_generate_k);
		ImGui::PushID("##generate_doubleEdged");
		ImGui::Checkbox("Double Edged",&m_generate_doubleEdged);
		ImGui::PopID();
		ImGui::InputFloat2("Lower Bound", m_generate_lowerBound);
		ImGui::InputFloat2("Upper Bound", m_generate_upperBound);

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

		if (ImGui::Button("Generate", ImVec2(100, 20))) { generateGraph(); }

		if (Singleton::currentlyProfiling()) { ImGui::EndDisabled(); }
		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (m_show) {
		float popupWidth = 320, popupHeight = 320;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Edit Graph", &m_show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		if (Singleton::currentlyProfiling()) { ImGui::BeginDisabled(); }

		if (ImGui::Button("Reset Graph", ImVec2(300, 20))) {
			Singleton::graph() = DirectedGraph<Vec2, float>(); Singleton::path() = Path();
			m_setNode_message.clear();
			m_setEdge_message.clear();
		}
		ImGui::SetItemTooltip("Remove all nodes and edges.");

		ImGui::Text("Add or Edit Node");
		ImGui::Separator();
		{
			if (ImGui::InputInt("Index", &m_setNode_index)) {
				if (graph.has(m_setNode_index)) {
					auto& node = graph.at(m_setNode_index);
					m_setNode_inputPosition[0] = node.value().x;
					m_setNode_inputPosition[1] = node.value().y;
					m_setNode_message.clear();
				}
				else {
					m_setNode_inputPosition[0] = 0.f;
					m_setNode_inputPosition[1] = 0.f;
					m_setNode_message.setMessage("Invalid index", true);
				}
			}
			ImGui::InputFloat2("Position", m_setNode_inputPosition);
			if (ImGui::Button("Set", ImVec2(100, 20))) {
				if (graph.has(m_setNode_index)) {
					graph.setValue(m_setNode_index, Vec2(m_setNode_inputPosition[0], m_setNode_inputPosition[1]));
					Singleton::recalculateEdgeWeights();
					Singleton::path().clear();
					m_setNode_message.setMessage("Set position of node " + std::to_string(m_setNode_index));
				}
				else {
					m_setNode_message.setMessage("Invalid index, could not set", true);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Add", ImVec2(100, 20))) {
				graph.createNode(Vec2(m_setNode_inputPosition[0], m_setNode_inputPosition[1]));
				m_setNode_index = graph.size() - 1;
				Singleton::path().clear();
				m_setNode_message.setMessage("Added node at index " + std::to_string(m_setNode_index));
			}
			m_setNode_message.draw();
		}
		ImGui::NewLine();
		ImGui::Text("Add or Edit Edge");
		ImGui::Separator();
		{
			if (ImGui::InputInt("Start Index", &m_setEdge_startIndex)) {
				if (graph.has(m_setEdge_startIndex)) { m_setEdge_message.clear(); }
				else { m_setEdge_message.setMessage("Invalid index", true); }
			}
			if (ImGui::InputInt("End Index", &m_setEdge_endIndex)) {
				if (graph.has(m_setEdge_endIndex)) { m_setEdge_message.clear(); }
				else { m_setEdge_message.setMessage("Invalid index", true); }
			}
			ImGui::Checkbox("Both Ways", &m_setEdge_doubleEdge);
			ImGui::PushID("set_edge");
			if (ImGui::Button("Set", ImVec2(100, 20))) {
				if (graph.has(m_setEdge_startIndex) && graph.has(m_setEdge_endIndex)) {
					if (m_setEdge_startIndex != m_setEdge_endIndex) {
						graph.setEdgeWeight(m_setEdge_startIndex, m_setEdge_endIndex, 1.f, m_setEdge_doubleEdge);
						Singleton::recalculateEdgeWeights();
						Singleton::path().clear();
						if (m_setEdge_doubleEdge) {
							m_setEdge_message.setMessage("Created edges between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex));
						}
						else {
							m_setEdge_message.setMessage("Created directed edge between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex));
						}
					}
					else { m_setEdge_message.setMessage("Start and end indicies are the same", true); }
				}
				else { m_setEdge_message.setMessage("Invalid index, could not set", true); }
			}
			ImGui::PopID();
			ImGui::SameLine();
			if (ImGui::Button("Remove", ImVec2(100, 20))) {
				if (graph.has(m_setEdge_startIndex) && graph.has(m_setEdge_endIndex)) {
					if (m_setEdge_startIndex != m_setEdge_endIndex) {
						graph.removeEdge(m_setEdge_startIndex, m_setEdge_endIndex, m_setEdge_doubleEdge);
						Singleton::path().clear();
						if (m_setEdge_doubleEdge) {
							m_setEdge_message.setMessage("Removed edges between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex));
						}
						else {
							m_setEdge_message.setMessage("Removed directed edge between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex));
						}
					}
					else { m_setEdge_message.setMessage("Start and end indicies are the same", true); }
				}
				else { m_setEdge_message.setMessage("Invalid index, could not remove", true); }
			}
			m_setEdge_message.draw();
		}

		if (Singleton::currentlyProfiling()) { ImGui::EndDisabled(); }
		ImGui::End();
		ImGui::PopStyleVar();
	}
}

void GraphEdit::imguiDisplayGraph() {
	auto getNodeInfo = [](const Vec2& val, const int& index) {
		NodeDisplayInfo info;
		info.name = std::to_string(index);
		info.pos.x = val.x; info.pos.y = val.y;
		return info;
		};

	if (m_showGraphAdjacencyTable) {
		if (ImGui::Button("Hide Adjacency Matrix", ImVec2(200, 20))) { m_showGraphAdjacencyTable = false; }
		DisplayAdjacencyTable<Vec2, float>(Singleton::graph(), Singleton::path(), getNodeInfo);
	}
	else {
		if (ImGui::Button("Show Adjacency Matrix", ImVec2(200, 20))) { m_showGraphAdjacencyTable = true; }
	}

	DisplayGraph<Vec2, float>(Singleton::graph(), Singleton::path(), getNodeInfo);
}