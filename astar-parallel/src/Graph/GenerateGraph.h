#pragma once

#include "DirectedGraph.h"
#include "../Maths/Vec2.h"
#include <functional>
#include <random>
#include <queue>

template<class distribution = std::uniform_real_distribution<float>>
DirectedGraph<Vec2, float> GenerateKNearest(int numNodes, int k, Vec2 min, Vec2 max, const std::function<float(const Vec2&, const Vec2&)>& distanceFunc, bool doubleEdged = false) {
	DirectedGraph<Vec2, float> graph;

	std::random_device rd;  // Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
	distribution x_distribution(min.x,max.x), y_distribution(min.y, max.y);

	for (int i = 0; i < numNodes; ++i) {
		graph.createNode(Vec2(x_distribution(gen), y_distribution(gen)));
	}

	struct indexDist { int index; float distance; indexDist(int index, float distance) : index(index), distance(distance) {} };
	auto comp = [](const indexDist& lhs, const indexDist& rhs) { return lhs.distance > rhs.distance; };

	for (int i = 0; i < numNodes; ++i) {
		Vec2 rootPos = graph.at(i).value();

		std::priority_queue<indexDist, std::vector<indexDist>, decltype(comp)> queue(comp);
		for (int j = 0; j < numNodes; ++j) {
			if (j == i) { continue; }
			Vec2 otherPos = graph.at(j).value();
			queue.push(indexDist(j, distanceFunc(rootPos, otherPos)));
		}
		for (int neighbour = 0; neighbour < k; ++neighbour) {
			if (queue.empty()) { break; }
			int endIndex = queue.top().index;
			queue.pop();
			graph.setEdgeWeight(i, endIndex, 1.f, doubleEdged);
		}
	}

	return graph;
}