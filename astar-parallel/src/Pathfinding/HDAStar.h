#pragma once

#include "../Graph/DirectedGraph.h"

#include <functional>
#include <queue>
#include <set>
#include <algorithm>
#include <thread>
#include <barrier>
#include <limits>
#include "Prototypes.h"

#include "MutexProtectedWrapper.h"

static int g_numThreads = std::thread::hardware_concurrency();

template<class Value, class Weight>
Path hashDistributedAStarSharedMemory(const DirectedGraph<Value, Weight>& graph, int start, int goal, const Heuristic<Value, Weight>& heuristicFunc) {
	if (graph.size() == 0) { return Path(); }

	// Find number of threads we will be using
	int numThreads;
	if (g_numThreads > 0) { numThreads = g_numThreads; } else { numThreads = std::thread::hardware_concurrency(); }

	// Hash function to assign indicies to threads
	auto hash = [numThreads](int index) { return index % numThreads; };

	// Vectors of mutex-protected values. Protected is just a simple wrapper class for accessing the values thread-safely
	std::vector<Protected<Weight>> costFromStart;
	std::vector<Protected<int>> parentIndex;
	// The h values don't change (we're just caching them), so they don't need thread protection
	std::vector<Weight> h; 
	costFromStart.reserve(graph.size()); parentIndex.reserve(graph.size()); h.reserve(graph.size());
	for (int i = 0; i < graph.size(); ++i) {
		costFromStart.emplace_back(std::numeric_limits<Weight>::max());
		parentIndex.emplace_back(-1);
		h.push_back(heuristicFunc(graph.at(i).value(), graph.at(goal).value()));
	}

	// f score to be used in open set ordering
	auto estimatedTotalCost = [&costFromStart, &h](int index) { return costFromStart.at(index).get() + h.at(index); };

	// Open sets are represented by a set ordered by lowest f score, protected by a mutex
	auto greaterEstimatedCost = [&estimatedTotalCost](const int& lhs, const int& rhs) { return estimatedTotalCost(lhs) > estimatedTotalCost(rhs); };
	using open_set = std::set<int, decltype(greaterEstimatedCost)>;

	class ProtectedOpenSet
	{
	private:
		open_set m_set;
		std::reference_wrapper<std::vector<Protected<Weight>>> m_costFromStartRef;
		std::mutex m_mutex;
	public:
		ProtectedOpenSet(open_set&& rhsSet, std::reference_wrapper<std::vector<Protected<Weight>>>&& costRef) : m_set(std::move(rhsSet)), m_costFromStartRef(costRef), m_mutex(std::mutex()) {}
		ProtectedOpenSet(const ProtectedOpenSet& other) : m_set(other.m_set), m_costFromStartRef(other.m_costFromStartRef), m_mutex(std::mutex()) {}

		// Pop top value from set and return it
		int pop() {
			auto lock = std::lock_guard(m_mutex);
			auto top = m_set.begin(); int index = *top;
			m_set.erase(top);
			return index;
		}

		bool isEmpty() { auto lock = std::lock_guard(m_mutex); return m_set.empty(); }

		// Set cost at index then push index to open set
		void setCostAndPush(int index, Weight newCost) {
			// Have to erase index before setting cost, since the set's ordering is dependent on cost
			// so it could otherwise break strict weak ordering and crash when an index was traversed more than once.
			// (This is why this version uses a std::set instead of a std::priority_queue, which can only pop from the top) 

			auto lock = std::lock_guard(m_mutex);
			m_set.erase(index);
			m_costFromStartRef.get().at(index).set(newCost);
			m_set.insert(index);
		}
	};

	// Vector of open sets, one per thread
	std::vector<ProtectedOpenSet> openSets;
	openSets.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) { openSets.emplace_back(open_set(greaterEstimatedCost), std::ref(costFromStart)); }

	// Set start cost to zero, push start index
	openSets[hash(start)].setCostAndPush(start, 0);

	// Barrier which threads arrive at when they run out of tasks
	bool allWorkComplete = false;
	std::barrier ranOutOfWorkBarrier(numThreads,
		[&]() noexcept {
			for (int i = 0; i < numThreads; ++i) {
				// Some threads still have work to do
				if (!openSets[i].isEmpty()) { return; }
			}
			// All threads have finished
			allWorkComplete = true;
		});

	auto threadFunc = [&](int threadIndex) {
		auto& openSet = openSets.at(threadIndex);
		do {
			while (!openSet.isEmpty()) {
				// Top of our open set
				int current = openSet.pop();

				Weight costCurrent = costFromStart[current].get();
				const std::map<int, Weight>& adjacencyMap = graph.at(current).adjacencyMap();

				// For each neighbour of current
				for (auto& [neighbour, edgeWeight] : adjacencyMap) {
					Weight tentativeNeighbourCost = costCurrent + edgeWeight;
					
					if (tentativeNeighbourCost < costFromStart[neighbour].get()) {
						parentIndex[neighbour].set(current);

						// Set neighbour's cost to new value, then push to relevant open set
						openSets[hash(neighbour)].setCostAndPush(neighbour, tentativeNeighbourCost);
					}
				}
			}
			ranOutOfWorkBarrier.arrive_and_wait();
		} while (!allWorkComplete);
	};

	// Create worker threads
	std::vector<std::thread> threads;
	threads.reserve(numThreads);
	for (int i = 0; i < numThreads; ++i) { threads.emplace_back(threadFunc, i); }

	// Wait for all worker threads to complete
	for (auto& thread : threads) { thread.join(); }

	// Reconstruct path from goal back to start
	Path path; path.push_back(goal);
	int prev = goal;
	while (prev != start) {
		// Fail state (no path found)
		if (prev == -1) { return Path(); }
		prev = parentIndex.at(prev).get();
		path.push_back(prev);
	}
	// Reverse so that it runs from start to goal
	std::reverse(path.begin(), path.end());
	return path;
}