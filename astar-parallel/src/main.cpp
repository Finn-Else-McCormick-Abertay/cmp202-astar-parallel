#include "Graph/DirectedGraph.h"
#include "Graph/GraphDisplay.h"

#include <vector>

#include "Window/Window.h"

DirectedGraph<NodeDisplayInfo, int>* g_graph = nullptr;

void imguiUpdate() {
	if (g_graph) {
		DisplayGraph<NodeDisplayInfo,int>(*g_graph, [](NodeDisplayInfo val, int index){ return val; });
	}
}

int main() {
	DirectedGraph<NodeDisplayInfo, int> graph(
		{
		{"a",{0,0}},
		{"b",{-1,-2}},
		{"c",{1,-2}},
		{"d",{-1,-4}},
		{"e",{1,-4}},
		{"f",{0,-6}}
		},
		{ {0,1},{1,5},{5,4},{4,1},{4,2},{2,3},{2,0},{2,5} }
	);
	g_graph = &graph;

	if (Window::init(imguiUpdate)) {
		Window::enterLoop();
	}
}