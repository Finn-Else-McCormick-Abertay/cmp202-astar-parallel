#include "Graph/DirectedGraph.h"
#include "Graph/GraphDisplay.h"

#include <vector>

#include "Window/Window.h"

#include "Pathfinding/AStar.h"
#include <iostream>

#include "Maths/Vec2.h"

DirectedGraph<Vec2, float>* g_graph = nullptr;
std::vector<int> path;

void imguiUpdate() {
	if (g_graph) {
		DisplayGraph<Vec2, float>(*g_graph, path,
			[](const Vec2& val, const int& index){
				NodeDisplayInfo info;
				info.name = std::to_string(index);
				info.pos.x = val.x; info.pos.y = val.y;
				return info;
			}
		);
	}
}

int main() {
	DirectedGraph<Vec2, float> graph;
	std::vector<Vec2> nodes = { {0,0}, {-1,-2}, {1,-2}, {-1,-4}, {1,-4}, {0,-6} };
	for (auto& node : nodes) { graph.createNode(node); }

	std::vector<std::pair<int,int>> edges = { {0,1},{1,5},{5,4},{4,1},{4,2},{2,3},{2,0},{2,5} };
	for (auto& pair : edges) {
		Vec2 pos1 = graph.at(pair.first).value();
		Vec2 pos2 = graph.at(pair.second).value();
		graph.setEdgeWeight(pair.first, pair.second, (pos2 - pos1).length(), true);
	}
	
	g_graph = &graph;

	path = aStarSinglethreaded<Vec2,float>(graph, 0, 4, [](const Vec2& val, const Vec2& goalVal) { return (val - goalVal).length(); });

	for (auto& index : path) {
		std::cout << index << " " << graph.at(index).value() << std::endl;
	}

	if (Window::init(imguiUpdate)) {
		Window::enterLoop();
	}
}