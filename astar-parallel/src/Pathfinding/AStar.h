#pragma once

#include "../Graph/DirectedGraph.h"

#include <functional>
#include <queue>
#include <algorithm>
#include <limits>
#include "Prototypes.h"

template<class Value, class Weight>
Path aStarSequential(const DirectedGraph<Value,Weight>& graph, int start, int goal, const Heuristic<Value,Weight>& heuristicFunc) {
	if (graph.size() == 0) { return Path(); }

	// Shorthand for calling heuristic at a given index
	auto h = [&](int index) { return heuristicFunc(graph.at(index).value(), graph.at(goal).value()); };

	// Vectors initialised to the same size as the graph so they can be easily indexed
	std::vector<Weight> costFromStart, estimatedTotalCost;
	std::vector<int> parentIndex;
	costFromStart.reserve(graph.size()); estimatedTotalCost.reserve(graph.size()); parentIndex.reserve(graph.size());
	for (int i = 0; i < graph.size(); ++i) {
		// Init g and f values to max value so new values will always be less
		costFromStart.push_back(std::numeric_limits<Weight>::max());
		estimatedTotalCost.push_back(std::numeric_limits<Weight>::max());
		parentIndex.push_back(-1);
	}
	// Set weight at start index to zero
	costFromStart[start] = 0; estimatedTotalCost[start] = h(start);

	// Open set is represented by a priority queue ordered by lowest f score of index
	auto greaterEstimatedCost = [&estimatedTotalCost](const int& lhs, const int& rhs) { return estimatedTotalCost.at(lhs) > estimatedTotalCost.at(rhs); };
	std::priority_queue<int, std::vector<int>, decltype(greaterEstimatedCost)> openSet(greaterEstimatedCost);

	// Push start index
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
				prev = parentIndex.at(prev);
				path.push_back(prev);
			}
			// Reverse so that it runs from start to goal
			std::reverse(path.begin(),path.end());
			return path;
		}

		// Remove current from open set
		openSet.pop();

		// For each neighbour of current
		const std::map<int, Weight>& adjacencyMap = graph.at(current).adjacencyMap();

		for (auto& [neighbour, edgeWeight] : adjacencyMap) {
			Weight tentativeNeighbourCost = costFromStart.at(current) + edgeWeight;

			if (tentativeNeighbourCost < costFromStart.at(neighbour)) {
				parentIndex[neighbour] = current;

				// Update cost
				costFromStart[neighbour] = tentativeNeighbourCost;
				estimatedTotalCost[neighbour] = tentativeNeighbourCost + h(neighbour);

				// Push neighbour to open set
				openSet.push(neighbour);
			}
		}
	}

	// Fail state
	return Path();
}