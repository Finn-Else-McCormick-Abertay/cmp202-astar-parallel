#include "Graph/DirectedGraph.h"

#include <iostream>

template<class ValueType, class WeightType>
void outputGraph(const DirectedGraph<ValueType, WeightType>& graph) {
	int size = graph.nodeCount();

	for (int i = 0; i < size; i++) {
		auto& node = graph.at(i);
		std::cout << "[" << i << "] (" << node.value << ") -> ";
		for (auto [index, weight] : node.adjacencyMap) {
			std::cout << "(" << index << ", " << weight << "), ";
		}
		std::cout << '\n';
	}
}

int main() {
	DirectedGraph<int, int> graph({ 5,61,4,5 }, { {0,1,0},{3,1,5} });

	outputGraph(graph);

	return 0;
}