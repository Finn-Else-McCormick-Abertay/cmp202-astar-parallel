#pragma once

#include "../Graph/DirectedGraph.h"
#include <functional>
#include <vector>

using Path = std::vector<int>;

template<typename ValueType, typename WeightType>
using Heuristic = std::function<WeightType(const ValueType&, const ValueType&)>;

template<typename ValueType, typename WeightType>
using PathfindingAlgorithm = std::function<Path(const DirectedGraph<ValueType, WeightType>&, int, int, const Heuristic<ValueType,WeightType>&)>;