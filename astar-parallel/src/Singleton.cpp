#include "Singleton.h"

#include <memory>

Singleton& Singleton::GetInstance() {
	static std::unique_ptr<Singleton> ptr(new Singleton);
	return *ptr;
}

DirectedGraph<Vec2, float>& Singleton::graph() {
	return GetInstance().m_graph;
}

Path& Singleton::path() {
	return GetInstance().m_path;
}

bool& Singleton::currentlyProfiling() {
	return GetInstance().m_currentlyProfiling;
}

void Singleton::recalculateEdgeWeights() {
	auto& g = graph();
	for (int i = 0; i < g.size(); ++i) {
		Vec2 pos1 = g.at(i).value();
		auto& map = g.at(i).adjacencyMap();
		for (auto& [j, weight] : map) {
			Vec2 pos2 = g.at(j).value();
			g.setEdgeWeight(i, j, (pos2 - pos1).length());
		}
	}
}