#pragma once
#include "Graph/DirectedGraph.h"
#include "Maths/Vec2.h"
#include "Pathfinding/Prototypes.h"
#include <iostream>

class Singleton
{
public:
	static DirectedGraph<Vec2, float>& graph();
	static Path& path();

	static void recalculateEdgeWeights();

	static bool& currentlyProfiling();

	template<typename ...Args>
	inline static void consoleOutput(Args... args) {
		(std::cout << ... << args) << std::endl;
	}
	inline static void consoleOutput() {
		std::cout << std::endl;
	}

	inline static std::ostream& getConsole() { return std::cout; }

private:
	Singleton() = default;

	static Singleton& GetInstance();

	DirectedGraph<Vec2, float> m_graph;
	Path m_path;

	bool m_currentlyProfiling = false;
};