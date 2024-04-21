#pragma once

#include "../Graph/DirectedGraph.h"

#include <functional>
#include <queue>
#include <map>

template<class ValueType, class WeightType>
std::vector<int> aStarSinglethreaded(
	const DirectedGraph<ValueType,WeightType>& graph, int start, int goal, const std::function<WeightType(const ValueType& nodeVal, const ValueType& goalVal)>& h) {

	std::priority_queue<int, std::vector<int>, std::greater<int>> openSet;
	openSet.push(start);

	ValueType goalVal = graph.at(goal).value();

	std::map<int, WeightType> g;
	g[start] = 0;

	std::map<int, WeightType> f;
	f[start] = h(graph.at(start).value(), goalVal);

	std::map<int, WeightType> cameFrom;

	while (!openSet.empty()) {
		int current = openSet.top();
		if (current == goal) {
			std::vector<int> reversePath;
			int prev = current;
			while (prev != start) {
				reversePath.push_back(prev);
				prev = cameFrom.at(prev);
			}
			reversePath.push_back(start);
			std::vector<int> finalPath; finalPath.reserve(reversePath.size());
			for (auto it = reversePath.cend(); it != reversePath.cbegin();) {
				--it;
				finalPath.push_back(*it);
			}
			return finalPath;
		}

		openSet.pop();
		const std::map<int, WeightType>& adjacencyMap = graph.at(current).adjacencyMap();

		for (auto& [neighbour, weight] : adjacencyMap) {
			WeightType g_tentative = g.at(current) + weight;
			bool replace = false;
			if (!g.contains(neighbour)) { replace = true; }
			else if (g_tentative < g.at(neighbour)) { replace = true; }

			if (replace) {
				cameFrom[neighbour] = current;
				g[neighbour] = g_tentative;
				f[neighbour] = g_tentative + h(graph.at(neighbour).value(), goalVal);
				openSet.push(neighbour);
			}
		}
	}

	// Failure
	return std::vector<int>();
}