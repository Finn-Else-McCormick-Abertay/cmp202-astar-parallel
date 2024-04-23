#pragma once

#include <string>
#include <functional>
#include "../Maths/Vec2.h"
#include "../Pathfinding/AStar.h"

class PathfindingSettings
{
public:
	PathfindingSettings();

	void addMenuBarItem();
	void imguiDrawWindow(int width, int height);
	void imguiDrawControlGroup();

	bool findPath();

	void setAlgorithm(int);
	void setHeuristic(int);

	void setIndices(int, int);

private:
	bool m_showSettingsDialog = false;
	int m_startIndex = 0;
	int m_goalIndex = 0;
	std::pair<std::string, bool> m_groupMessage;

	std::vector<std::pair<std::function<float(const Vec2&, const Vec2&)>, std::string>> m_heuristics;
	int m_heuristicIndex = 0;

	std::vector<std::pair<std::function<std::vector<int>(const DirectedGraph<Vec2,float>&,int,int,const std::function<float(const Vec2&,const Vec2&)>&)>, std::string>> m_algorithms;
	int m_algorithmIndex = 1;

	bool m_showProfilingDialog = false;
};