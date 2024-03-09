#pragma once

#include "../Graph/DirectedGraph.h"

#include <functional>
#include <vector>

template<class ValueType, class WeightType = int>
std::vector<int> aStarSinglethreaded(const DirectedGraph<ValueType,WeightType>& graph, int start, int goal, std::function<WeightType(const ValueType&)>& h) {
	std::vector<int> finalPath;


	return finalPath;
}