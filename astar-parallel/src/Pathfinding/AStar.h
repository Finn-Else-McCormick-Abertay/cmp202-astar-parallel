#pragma once

#include "../Graph/DirectedGraph.h"

#include <functional>
#include <queue>
#include <algorithm>
#include <thread>
#include <mutex>
#include <barrier>

#include <limits>

template<class ValueType, class WeightType>
std::vector<int> aStarSinglethreaded(
	const DirectedGraph<ValueType,WeightType>& graph, int start, int goal, const std::function<WeightType(const ValueType& nodeVal, const ValueType& goalVal)>& heuristicFunc) {

	auto h = [&](int index) { return heuristicFunc(graph.at(index).value(), graph.at(goal).value()); };

	std::vector<WeightType> g, f; std::vector<int> cameFrom;
	// Init g and f values to max value so new values will always be less
	g.reserve(graph.size()); f.reserve(graph.size()); cameFrom.reserve(graph.size());
	for (int i = 0; i < graph.size(); ++i) { g.push_back(std::numeric_limits<WeightType>::max()); f.push_back(std::numeric_limits<WeightType>::max()); cameFrom.push_back(-1); }
	g[start] = 0; f[start] = h(start);

	auto f_compare = [&f](const int& lhs, const int& rhs) { return f.at(lhs) > f.at(rhs); };

	// Opens set is represented by a priority queue ordered by lowest f score of index
	std::priority_queue<int, std::vector<int>, decltype(f_compare)> openSet(f_compare);
	openSet.push(start);

	while (!openSet.empty()) {
		// Node in the open set with lowest f score
		int current = openSet.top();

		// Goal found
		if (current == goal) {
			// Reconstruct path from goal back to start
			std::vector<int> path;
			path.push_back(current);
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
	return std::vector<int>();
}


static int g_numThreads = std::thread::hardware_concurrency();

template<class ValueType, class WeightType>
std::vector<int> hashDistributedAStarSharedMemory(
	const DirectedGraph<ValueType, WeightType>& graph, int start, int goal, const std::function<WeightType(const ValueType& nodeVal, const ValueType& goalVal)>& heuristicFunc) {

	auto h = [&](int index) { return heuristicFunc(graph.at(index).value(), graph.at(goal).value()); };

	struct ProtectedWeight {
		WeightType weight; std::mutex mutex;
		ProtectedWeight(WeightType weight) : weight(weight), mutex(std::mutex()) {}
		ProtectedWeight(const ProtectedWeight& other) : weight(other.weight), mutex(std::mutex()) {}
	};
	std::vector<ProtectedWeight> g;

	struct ProtectedIndex {
		int index; std::mutex mutex;
		ProtectedIndex(int index) : index(index), mutex(std::mutex()) {}
		ProtectedIndex(const ProtectedIndex& other) : index(other.index), mutex(std::mutex()) {}
	};
	std::vector<ProtectedIndex> cameFrom;

	g.reserve(graph.size()); cameFrom.reserve(graph.size());
	for (int i = 0; i < graph.size(); ++i) {
		g.emplace_back(std::numeric_limits<WeightType>::max());
		cameFrom.emplace_back(-1);
	}

	// Create vector of threads, to be filled later
	int numThreads; if (g_numThreads > 0) { numThreads = g_numThreads; } else { numThreads = std::thread::hardware_concurrency(); }
	std::vector<std::thread> threads; threads.reserve(numThreads);

	// Hash function to assign indicies to threads
	auto threadHash = [numThreads](int index) { return index % numThreads; };

	auto f_compare = [&g, &h](const int& lhs, const int& rhs) { return (g.at(lhs).weight + h(lhs)) > (g.at(rhs).weight + h(rhs)); };

	struct ProtectedOpenSet
	{
		std::priority_queue<int, std::vector<int>, decltype(f_compare)> set; std::mutex mutex;
		ProtectedOpenSet(std::priority_queue<int, std::vector<int>, decltype(f_compare)>&& rhsSet) : set(std::move(rhsSet)), mutex(std::mutex()) {}
		ProtectedOpenSet(const ProtectedOpenSet& other) : set(other.set), mutex(std::mutex()) {}
	};

	// Each thread has its own open set, each with a corresponding mutex
	std::vector<ProtectedOpenSet> openSets; openSets.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) {
		openSets.emplace_back(std::priority_queue<int, std::vector<int>, decltype(f_compare)>(f_compare));
	}

	auto pushToOwnerOpenSet = [&](int index) {
		int threadIndex = threadHash(index);
		auto openSetLock = std::lock_guard(openSets[threadIndex].mutex);
		openSets[threadIndex].set.push(index);
		};

	auto isSetEmpty = [](ProtectedOpenSet& set) {
		auto lock = std::lock_guard(set.mutex);
		return set.set.empty();
		};

	bool hasFinished = false;
	auto barrierCompleteFunc = [&]() noexcept {
		for (int i = 0; i < numThreads; ++i) {
			// Some threads still have work to do
			if (!openSets[i].set.empty()) { return; }
		}
		// All threads have finished
		hasFinished = true;
		};
	std::barrier finishedWorkBarrier(numThreads, barrierCompleteFunc);

	auto threadFunc = [&](int index) {
		auto& protectedOpenSet = openSets.at(index);
		do {
			while (!isSetEmpty(protectedOpenSet)) {
				int current;
				{
					auto openSetLock = std::lock_guard(protectedOpenSet.mutex);
					current = protectedOpenSet.set.top();
					protectedOpenSet.set.pop();
				}

				WeightType current_g;
				{
					auto gLock = std::lock_guard(g[current].mutex);
					current_g = g.at(current).weight;
				}

				const std::map<int, WeightType>& adjacencyMap = graph.at(current).adjacencyMap();

				// For each neighbour of current
				for (auto& [neighbour, weight] : adjacencyMap) {
					WeightType neighbour_g_tentative = current_g + weight;
					bool shouldPush = false;
					{
						auto neighbourGLock = std::lock_guard(g[neighbour].mutex);
						if (neighbour_g_tentative < g.at(neighbour).weight) {
							auto cameFromLock = std::lock_guard(cameFrom[neighbour].mutex);
							cameFrom[neighbour].index = current;
							g[neighbour].weight = neighbour_g_tentative;
							shouldPush = true;
						}
					}
					// Seperated out so locks release before attempting to push
					if (shouldPush) {
						pushToOwnerOpenSet(neighbour);
					}
				}
			}
			finishedWorkBarrier.arrive_and_wait();
		} while (!hasFinished);
		};

	// Push start index
	g[start].weight = 0;
	pushToOwnerOpenSet(start);

	// Create threads
	for (int i = 0; i < numThreads; ++i) { threads.emplace_back(threadFunc, i); }

	// Main thread waits for all worker threads to complete
	for (auto& thread : threads) { thread.join(); }

	// Reconstruct path from goal back to start
	std::vector<int> path;
	path.push_back(goal);
	int prev = goal;
	while (prev != start) {
		// Fail condition
		if (prev == -1) { return std::vector<int>(); }
		prev = cameFrom.at(prev).index;
		path.push_back(prev);
	}
	// Reverse so that it runs from start to goal
	std::reverse(path.begin(), path.end());
	return path;
}