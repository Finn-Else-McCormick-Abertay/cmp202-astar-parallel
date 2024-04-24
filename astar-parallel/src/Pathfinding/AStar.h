#pragma once

#include "../Graph/DirectedGraph.h"

#include <functional>
#include <queue>
#include <algorithm>
#include <limits>
#include "Prototypes.h"

template<class ValueType, class WeightType>
Path aStarSequential(const DirectedGraph<ValueType,WeightType>& graph, int start, int goal, const Heuristic<ValueType,WeightType>& heuristicFunc) {
	if (graph.size() == 0) { return Path(); }

	// Shorthand for calling heuristic at a given index
	auto h = [&](int index) { return heuristicFunc(graph.at(index).value(), graph.at(goal).value()); };

	std::vector<WeightType> g, f; std::vector<int> cameFrom;
	g.reserve(graph.size()); f.reserve(graph.size()); cameFrom.reserve(graph.size());
	// Init g and f values to max value so new values will always be less
	for (int i = 0; i < graph.size(); ++i) {
		g.push_back(std::numeric_limits<WeightType>::max());
		f.push_back(std::numeric_limits<WeightType>::max());
		cameFrom.push_back(-1);
	}
	g[start] = 0; f[start] = h(start);

	// Open set is represented by a priority queue ordered by lowest f score of index
	auto f_compare = [&f](const int& lhs, const int& rhs) { return f.at(lhs) > f.at(rhs); };
	std::priority_queue<int, std::vector<int>, decltype(f_compare)> openSet(f_compare);
	openSet.push(start);

	while (!openSet.empty()) {
		// Node in the open set with lowest f score
		int current = openSet.top();

		// Goal found
		if (current == goal) {
			// Reconstruct path from goal back to start
			Path path; path.push_back(current);
			int prev = current;
			while (prev != start) {
				prev = cameFrom.at(prev);
				path.push_back(prev);
			}
			// Reverse so that it runs from start to goal
			std::reverse(path.begin(),path.end());
			return path;
		}

		// Remove current from open set
		openSet.pop();
		const std::map<int, WeightType>& adjacencyMap = graph.at(current).adjacencyMap();

		// For each neighbour of current
		for (auto& [neighbour, weight] : adjacencyMap) {
			WeightType neighbour_g_tentative = g.at(current) + weight;

			if (neighbour_g_tentative < g.at(neighbour)) {
				cameFrom[neighbour] = current;
				g[neighbour] = neighbour_g_tentative;
				f[neighbour] = neighbour_g_tentative + h(neighbour);
				openSet.push(neighbour);
			}
		}
	}

	// Failure
	return Path();
}