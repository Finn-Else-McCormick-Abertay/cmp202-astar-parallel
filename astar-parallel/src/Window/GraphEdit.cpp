#include "GraphEdit.h"
#include "../../imgui/imgui.h"

#include "../Singleton.h"
#include <string>

#include "../Graph/GraphJSON.h"

#include "../Graph/GraphDisplay.h"

void GraphEdit::saveGraph(std::string path) {
	if (saveToFile(Singleton::graph(), path)) {
		m_saveLoadMessage.first = "Saved to path " + std::string(m_inputSavePath);
		m_saveLoadMessage.second = false;
	}
	else {
		m_saveLoadMessage.first = "Failed to save to path " + std::string(m_inputSavePath);
		m_saveLoadMessage.second = true;
	}
}

void GraphEdit::loadGraph(std::string path) {
	auto [newGraph, valid] = loadFromFile<Vec2, float>(path);
	if (valid) {
		Singleton::graph() = newGraph;
		Singleton::path() = std::vector<int>();
		m_saveLoadMessage.first = "Loaded from path " + std::string(m_inputSavePath);
		m_saveLoadMessage.second = false;
	}
	else {
		m_saveLoadMessage.first = "No such file " + std::string(m_inputSavePath);
		m_saveLoadMessage.second = true;
	}
}

void GraphEdit::addMenuBarItem() {
	if (ImGui::BeginMenu("Graph")) {
		if (ImGui::MenuItem("Edit")) {
			m_show = true;
			ImGui::SetWindowFocus("Edit Graph");
		}
		if (ImGui::MenuItem("Reset")) {
			Singleton::graph() = DirectedGraph<Vec2, float>();
			Singleton::path() = std::vector<int>();
		}
		if (ImGui::MenuItem("Save")) {
			if (!m_saveLoadDialogIsSave) { m_saveLoadMessage.first = ""; }
			m_showSaveLoadDialog = true;
			m_saveLoadDialogIsSave = true;
			ImGui::SetWindowFocus("Save");
		}
		if (ImGui::MenuItem("Load")) {
			if (m_saveLoadDialogIsSave) { m_saveLoadMessage.first = ""; }
			m_showSaveLoadDialog = true;
			m_saveLoadDialogIsSave = false;
			ImGui::SetWindowFocus("Load");
		}
		ImGui::EndMenu();
	}
}

void GraphEdit::imguiMessage(const std::pair<std::string, bool>& message) {
	ImColor textColor = message.second ? ImColor(1.f, 0.1f, 0.1f) : ImColor(0.f, 0.8f, 0.2f);
	ImGui::TextColored(textColor, "%s", message.first.c_str());
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

		auto buttonOrEnterPressed = [&]() {
			if (m_saveLoadDialogIsSave) { saveGraph(std::string(m_inputSavePath)); }
			else { loadGraph(std::string(m_inputSavePath)); }
			};

		ImGui::SetNextItemWidth(180);
		if (ImGui::InputText("##filepathInput", m_inputSavePath, IM_ARRAYSIZE(m_inputSavePath), ImGuiInputTextFlags_EnterReturnsTrue)) {
			buttonOrEnterPressed();
		}
		if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)) { ImGui::SetKeyboardFocusHere(-1); }
		ImGui::PushID("##saveloadbutton");
		if (ImGui::Button(label, ImVec2(100, 20))) {
			buttonOrEnterPressed();
		}
		ImGui::PopID();
		imguiMessage(m_saveLoadMessage);
		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (m_show) {
		float popupWidth = 320, popupHeight = 290;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Edit Graph", &m_show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::Text("Add or Edit Node");
		ImGui::Separator();
		{
			if (ImGui::InputInt("Index", &m_setNode_index)) {
				if (graph.has(m_setNode_index)) {
					auto& node = graph.at(m_setNode_index);
					m_setNode_inputPosition[0] = node.value().x;
					m_setNode_inputPosition[1] = node.value().y;
					m_setNode_message.first = "";
				}
				else {
					m_setNode_inputPosition[0] = 0.f;
					m_setNode_inputPosition[1] = 0.f;
					m_setNode_message.first = "Invalid index";
					m_setNode_message.second = true;
				}
			}
			ImGui::InputFloat2("Position", m_setNode_inputPosition);
			if (ImGui::Button("Set", ImVec2(100, 20))) {
				if (graph.has(m_setNode_index)) {
					graph.setValue(m_setNode_index, Vec2(m_setNode_inputPosition[0], m_setNode_inputPosition[1]));
					Singleton::recalculateEdgeWeights();
					Singleton::path().clear();
					m_setNode_message.first = "Set position of node " + std::to_string(m_setNode_index);
					m_setNode_message.second = false;
				}
				else {
					m_setNode_message.first = "Invalid index, could not set";
					m_setNode_message.second = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Add", ImVec2(100, 20))) {
				graph.createNode(Vec2(m_setNode_inputPosition[0], m_setNode_inputPosition[1]));
				m_setNode_index = graph.size() - 1;
				Singleton::path().clear();
				m_setNode_message.first = "Added node at index " + std::to_string(m_setNode_index);
				m_setNode_message.second = false;
			}
			imguiMessage(m_setNode_message);
		}
		ImGui::NewLine();
		ImGui::Text("Add or Edit Edge");
		ImGui::Separator();
		{
			if (ImGui::InputInt("Start Index", &m_setEdge_startIndex)) {
				if (graph.has(m_setEdge_startIndex)) {
					m_setEdge_message.first = "";
				}
				else {
					m_setEdge_message.first = "Invalid index";
					m_setEdge_message.second = true;
				}
			}
			if (ImGui::InputInt("End Index", &m_setEdge_endIndex)) {
				if (graph.has(m_setEdge_endIndex)) {
					m_setEdge_message.first = "";
				}
				else {
					m_setEdge_message.first = "Invalid index";
					m_setEdge_message.second = true;
				}
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
							m_setEdge_message.first = "Created edges between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex);
						}
						else {
							m_setEdge_message.first = "Created directed edge between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex);
						}
						m_setEdge_message.second = false;
					}
					else {
						m_setEdge_message.first = "Start and end indicies are the same";
						m_setEdge_message.second = true;
					}
				}
				else {
					m_setEdge_message.first = "Invalid index, could not set";
					m_setEdge_message.second = true;
				}
			}
			ImGui::PopID();
			ImGui::SameLine();
			if (ImGui::Button("Remove", ImVec2(100, 20))) {
				if (graph.has(m_setEdge_startIndex) && graph.has(m_setEdge_endIndex)) {
					if (m_setEdge_startIndex != m_setEdge_endIndex) {
						graph.removeEdge(m_setEdge_startIndex, m_setEdge_endIndex, m_setEdge_doubleEdge);
						Singleton::path().clear();
						if (m_setEdge_doubleEdge) {
							m_setEdge_message.first = "Removed edges between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex);
						}
						else {
							m_setEdge_message.first = "Removed directed edge between nodes "
								+ std::to_string(m_setEdge_startIndex) + " and " + std::to_string(m_setEdge_endIndex);
						}
						m_setEdge_message.second = false;
					}
					else {
						m_setEdge_message.first = "Start and end indicies are the same";
						m_setEdge_message.second = true;
					}
				}
				else {
					m_setEdge_message.first = "Invalid index, could not remove";
					m_setEdge_message.second = true;
				}
			}
			imguiMessage(m_setEdge_message);
		}

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