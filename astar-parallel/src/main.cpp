#include "Graph/DirectedGraph.h"
#include "Graph/GraphDisplay.h"

#include <vector>

#include "Window/Window.h"

#include "Pathfinding/AStar.h"
#include <iostream>

#include "Maths/Vec2.h"

DirectedGraph<Vec2, float> g_graph;
std::vector<int> g_path;

void recalculateEdgeWeights() {
	for (int i = 0; i < g_graph.nodeCount(); ++i) {
		Vec2 pos1 = g_graph.at(i).value();
		auto& map = g_graph.at(i).adjacencyMap();
		for (auto& [j, weight] : map) {
			Vec2 pos2 = g_graph.at(j).value();
			g_graph.setEdgeWeight(i, j, (pos2 - pos1).length());
		}
	}
}

bool g_showGraphEditMenu = false;
int g_graphEdit_setNode_inputIndex = 0;
float g_graphEdit_inputPosition[2] = {0,0};
std::string g_graphEdit_setNode_message; bool g_graphEdit_setNode_messageIsError = false;

int g_graphEdit_setEdge_inputStartIndex = 0;
int g_graphEdit_setEdge_inputEndIndex = 0;
bool g_graphEdit_graphDoubleEdge = true;
std::string g_graphEdit_setEdge_message; bool g_graphEdit_setEdge_messageIsError = false;


bool g_showPathfindingSettingsMenu = false;
int g_pathfindingStart = 0;
int g_pathfindingEnd = 0;

std::function<float(const Vec2&, const Vec2&)> g_pathfinding_heuristic_functions[] = {
	[](const Vec2& val, const Vec2& goalVal) { return (val - goalVal).length(); },
	[](const Vec2& val, const Vec2& goalVal) { return abs(val.x - goalVal.x) + abs(val.y - goalVal.y); }
};
const char* g_pathfinding_settings_heuristic_names[] = { "Straight Line Distance", "Manhattan Distance" };
int g_pathfinding_settings_heuristic_currentIndex = 0;

std::function<std::vector<int>(const DirectedGraph<Vec2, float>&, int, int, const std::function<float(const Vec2&, const Vec2&)>&)> g_pathfinding_algorithms[] = {
	aStarSinglethreaded<Vec2,float>
};
const char* g_pathfinding_settings_algorithm_names[] = { "A* Single Threaded" };
int g_pathfinding_settings_algorithm_currentIndex = 0;

std::string g_pathfinding_message; bool g_pathfinding_messageIsError = false;

void clearPath() {
	g_path = std::vector<int>();
}

bool findPath() {
	g_path = g_pathfinding_algorithms[g_pathfinding_settings_algorithm_currentIndex](
		g_graph, g_pathfindingStart, g_pathfindingEnd, g_pathfinding_heuristic_functions[g_pathfinding_settings_heuristic_currentIndex]);
	return g_path.size() > 0;
}

float imguiTitlebar() {
	float menuBarHeight = 0.f;
	if (ImGui::BeginMainMenuBar()) {
		menuBarHeight = ImGui::GetWindowHeight();
		if (ImGui::BeginMenu("Graph")) {
			if (ImGui::MenuItem("Edit")) {
				g_showGraphEditMenu = true;
				ImGui::SetWindowFocus("Edit Graph");
			}
			if (ImGui::MenuItem("Reset")) {
				g_graph = DirectedGraph<Vec2, float>();
				clearPath();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Pathfinding")) {
			if (ImGui::MenuItem("Settings")) {
				g_showPathfindingSettingsMenu = true;
				ImGui::SetWindowFocus("Pathfinding Settings");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	return menuBarHeight;
}

void imguiUpdate(int width, int height) {
	if (g_showGraphEditMenu) {
		float popupWidth = 320, popupHeight = 290;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Edit Graph", &g_showGraphEditMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::Text("Add or Edit Node");
		ImGui::Separator();
		{
			if (ImGui::InputInt("Index", &g_graphEdit_setNode_inputIndex)) {
				if (g_graph.has(g_graphEdit_setNode_inputIndex)) {
					auto& node = g_graph.at(g_graphEdit_setNode_inputIndex);
					g_graphEdit_inputPosition[0] = node.value().x;
					g_graphEdit_inputPosition[1] = node.value().y;
					clearPath();
					g_graphEdit_setNode_message = "";
				}
				else {
					g_graphEdit_inputPosition[0] = 0.f;
					g_graphEdit_inputPosition[1] = 0.f;
					g_graphEdit_setNode_message = "Invalid index";
					g_graphEdit_setNode_messageIsError = true;
				}
			}
			ImGui::InputFloat2("Position", g_graphEdit_inputPosition);
			if (ImGui::Button("Set", ImVec2(100, 20))) {
				if (g_graph.has(g_graphEdit_setNode_inputIndex)) {
					g_graph.setValue(g_graphEdit_setNode_inputIndex, Vec2(g_graphEdit_inputPosition[0], g_graphEdit_inputPosition[1]));
					recalculateEdgeWeights();
					clearPath();
					g_graphEdit_setNode_message = "Set position of node " + std::to_string(g_graphEdit_setNode_inputIndex);
					g_graphEdit_setNode_messageIsError = false;
				}
				else {
					g_graphEdit_setNode_message = "Invalid index, could not set";
					g_graphEdit_setNode_messageIsError = true;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Add", ImVec2(100, 20))) {
				g_graph.createNode(Vec2(g_graphEdit_inputPosition[0], g_graphEdit_inputPosition[1]));
				g_graphEdit_setNode_inputIndex = g_graph.nodeCount() - 1;
				clearPath();
				g_graphEdit_setNode_message = "Added node at index " + std::to_string(g_graphEdit_setNode_inputIndex);
				g_graphEdit_setNode_messageIsError = false;
			}
			ImColor textColor = g_graphEdit_setNode_messageIsError ? ImColor(1.f, 0.1f, 0.1f) : ImColor(0.f, 0.8f, 0.2f);
			ImGui::TextColored(textColor, "%s", g_graphEdit_setNode_message.c_str());
		}
		ImGui::NewLine();
		ImGui::Text("Add or Edit Edge");
		ImGui::Separator();
		{
			if (ImGui::InputInt("Start Index", &g_graphEdit_setEdge_inputStartIndex)) {
				if (g_graph.has(g_graphEdit_setEdge_inputStartIndex)) {
					g_graphEdit_setEdge_message = "";
				}
				else {
					g_graphEdit_setEdge_message = "Invalid index";
					g_graphEdit_setEdge_messageIsError = true;
				}
			}
			if (ImGui::InputInt("End Index", &g_graphEdit_setEdge_inputEndIndex)) {
				if (g_graph.has(g_graphEdit_setEdge_inputEndIndex)) {
					g_graphEdit_setEdge_message = "";
				}
				else {
					g_graphEdit_setEdge_message = "Invalid index";
					g_graphEdit_setEdge_messageIsError = true;
				}
			}
			ImGui::Checkbox("Both Ways", &g_graphEdit_graphDoubleEdge);
			ImGui::PushID("set_edge");
			if (ImGui::Button("Set", ImVec2(100, 20))) {
				if (g_graph.has(g_graphEdit_setEdge_inputStartIndex) && g_graph.has(g_graphEdit_setEdge_inputEndIndex)) {
					if (g_graphEdit_setEdge_inputStartIndex != g_graphEdit_setEdge_inputEndIndex) {
						g_graph.setEdgeWeight(g_graphEdit_setEdge_inputStartIndex, g_graphEdit_setEdge_inputEndIndex, 1.f, g_graphEdit_graphDoubleEdge);
						recalculateEdgeWeights();
						clearPath();
						if (g_graphEdit_graphDoubleEdge) {
							g_graphEdit_setEdge_message = "Created edges between nodes "
								+ std::to_string(g_graphEdit_setEdge_inputStartIndex) + " and " + std::to_string(g_graphEdit_setEdge_inputEndIndex);
						}
						else {
							g_graphEdit_setEdge_message = "Created directed edge between nodes "
								+ std::to_string(g_graphEdit_setEdge_inputStartIndex) + " and " + std::to_string(g_graphEdit_setEdge_inputEndIndex);
						}
						g_graphEdit_setEdge_messageIsError = false;
					}
					else {
						g_graphEdit_setEdge_message = "Start and end indicies are the same";
						g_graphEdit_setEdge_messageIsError = true;
					}
				}
				else {
					g_graphEdit_setEdge_message = "Invalid index, could not set";
					g_graphEdit_setEdge_messageIsError = true;
				}
			}
			ImGui::PopID();
			ImGui::SameLine();
			if (ImGui::Button("Remove", ImVec2(100, 20))) {
				if (g_graph.has(g_graphEdit_setEdge_inputStartIndex) && g_graph.has(g_graphEdit_setEdge_inputEndIndex)) {
					if (g_graphEdit_setEdge_inputStartIndex != g_graphEdit_setEdge_inputEndIndex) {
						g_graph.removeEdge(g_graphEdit_setEdge_inputStartIndex, g_graphEdit_setEdge_inputEndIndex, g_graphEdit_graphDoubleEdge);
						clearPath();
						if (g_graphEdit_graphDoubleEdge) {
							g_graphEdit_setEdge_message = "Removed edges between nodes "
								+ std::to_string(g_graphEdit_setEdge_inputStartIndex) + " and " + std::to_string(g_graphEdit_setEdge_inputEndIndex);
						}
						else {
							g_graphEdit_setEdge_message = "Removed directed edge between nodes "
								+ std::to_string(g_graphEdit_setEdge_inputStartIndex) + " and " + std::to_string(g_graphEdit_setEdge_inputEndIndex);
						}
						g_graphEdit_setEdge_messageIsError = false;
					}
					else {
						g_graphEdit_setEdge_message = "Start and end indicies are the same";
						g_graphEdit_setEdge_messageIsError = true;
					}
				}
				else {
					g_graphEdit_setEdge_message = "Invalid index, could not remove";
					g_graphEdit_setEdge_messageIsError = true;
				}
			}
			ImColor textColor = g_graphEdit_setEdge_messageIsError ? ImColor(1.f, 0.1f, 0.1f) : ImColor(0.f, 0.8f, 0.2f);
			ImGui::TextColored(textColor, "%s", g_graphEdit_setEdge_message.c_str());
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}

	if (g_showPathfindingSettingsMenu) {
		float popupWidth = 300, popupHeight = 130;
		ImGui::SetNextWindowPos({ width / 2.f - popupWidth / 2.f, height / 2.f - popupHeight / 2.f }, ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(popupWidth, popupHeight), ImGuiCond_Once);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.f);
		ImGui::Begin("Pathfinding Settings", &g_showPathfindingSettingsMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::Text("Algorithm");
		if (ImGui::BeginCombo("##algorithmCombo", g_pathfinding_settings_algorithm_names[g_pathfinding_settings_algorithm_currentIndex])) {
			for (int n = 0; n < IM_ARRAYSIZE(g_pathfinding_settings_algorithm_names); n++)
			{
				bool is_selected = (g_pathfinding_settings_algorithm_currentIndex == n);
				if (ImGui::Selectable(g_pathfinding_settings_algorithm_names[n], is_selected)) {
					g_pathfinding_settings_algorithm_currentIndex = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::Text("Heuristic");
		if (ImGui::BeginCombo("##heuristicCombo", g_pathfinding_settings_heuristic_names[g_pathfinding_settings_heuristic_currentIndex])) {
			for (int n = 0; n < IM_ARRAYSIZE(g_pathfinding_settings_heuristic_names); n++)
			{
				bool is_selected = (g_pathfinding_settings_heuristic_currentIndex == n);
				if (ImGui::Selectable(g_pathfinding_settings_heuristic_names[n], is_selected)) {
					g_pathfinding_settings_heuristic_currentIndex = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	{
		ImGui::BeginGroup();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt("Start", &g_pathfindingStart)) {
			if (g_graph.has(g_pathfindingStart)) {
				g_pathfinding_message = "";
			}
			else {
				g_pathfinding_message = "Invalid index";
				g_pathfinding_messageIsError = true;
			}
		}
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (ImGui::InputInt("Goal", &g_pathfindingEnd)) {
			if (g_graph.has(g_pathfindingEnd)) {
				g_pathfinding_message = "";
			}
			else {
				g_pathfinding_message = "Invalid index";
				g_pathfinding_messageIsError = true;
			}
		}
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();
		if (ImGui::Button("Find Path", ImVec2(100, 20))) {
			if (g_graph.has(g_pathfindingStart) && g_graph.has(g_pathfindingEnd)) {
				if (g_pathfindingStart != g_pathfindingEnd) {
					if (findPath()) {
						g_pathfinding_message = "";
					}
					else {
						g_pathfinding_message = "Path could not be found";
						g_pathfinding_messageIsError = true;
					}
				}
				else {
					g_pathfinding_message = "Start cannot be goal";
					g_pathfinding_messageIsError = true;
				}
			}
			else {
				g_pathfinding_message = "Invalid index";
				g_pathfinding_messageIsError = true;
			}
		}
		ImGui::SameLine();
		ImColor textColor = g_pathfinding_messageIsError ? ImColor(1.f, 0.1f, 0.1f) : ImColor(0.f, 0.8f, 0.2f);
		ImGui::TextColored(textColor, " %s", g_pathfinding_message.c_str());
		ImGui::EndGroup();
	}

	DisplayGraph<Vec2, float>(g_graph, g_path,
		[](const Vec2& val, const int& index){
			NodeDisplayInfo info;
			info.name = std::to_string(index);
			info.pos.x = val.x; info.pos.y = val.y;
			return info;
		}
	);
}

int main() {
	std::vector<Vec2> nodes = { {0,0}, {-1,-2}, {1,-2}, {-1,-4}, {1,-4}, {0,-6} };
	for (auto& node : nodes) { g_graph.createNode(node); }

	std::vector<std::pair<int,int>> edges = { {0,1},{1,5},{5,4},{4,1},{4,2},{2,3},{2,0},{2,5} };
	for (auto& pair : edges) {
		Vec2 pos1 = g_graph.at(pair.first).value();
		Vec2 pos2 = g_graph.at(pair.second).value();
		g_graph.setEdgeWeight(pair.first, pair.second, (pos2 - pos1).length(), true);
	}

	//g_path = aStarSinglethreaded<Vec2,float>(g_graph, 0, 4, [](const Vec2& val, const Vec2& goalVal) { return (val - goalVal).length(); });

	if (Window::init(imguiUpdate, imguiTitlebar)) {
		Window::enterLoop();
	}
}
