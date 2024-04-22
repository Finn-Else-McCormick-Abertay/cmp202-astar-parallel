#pragma once
#include "Graph/DirectedGraph.h"
#include "Maths/Vec2.h"

class Singleton
{
public:
	static DirectedGraph<Vec2, float>& graph();
	static std::vector<int>& path();

	static void recalculateEdgeWeights();

private:
	Singleton() = default;

	static Singleton& GetInstance();

	DirectedGraph<Vec2, float> m_graph;
	std::vector<int> m_path;
};