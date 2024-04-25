#pragma once
#include "Graph/DirectedGraph.h"
#include "Maths/Vec2.h"
#include "Pathfinding/Prototypes.h"
#include <string>

class Singleton
{
public:
	static DirectedGraph<Vec2, float>& graph();
	static Path& path();

	static void recalculateEdgeWeights();

	static bool& currentlyProfiling();

	static void consoleOutput(const std::string&);

private:
	Singleton() = default;

	static Singleton& GetInstance();

	DirectedGraph<Vec2, float> m_graph;
	Path m_path;

	bool m_currentlyProfiling = false;
};