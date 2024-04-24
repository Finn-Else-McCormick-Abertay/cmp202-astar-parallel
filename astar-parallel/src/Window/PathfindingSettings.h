#pragma once

#include <string>
#include <functional>
#include "../Maths/Vec2.h"
#include "../Pathfinding/Prototypes.h"
#include "../Profiling/Profiler.h"
#include <memory>

#include "ImGuiUtil.h"

class PathfindingSettings
{
public:
	PathfindingSettings();

	void addMenuBarItem();
	void imguiDrawWindow(int width, int height);
	void imguiDrawControlGroup();

	bool findPath();
	void startProfiling();

	void setAlgorithm(int);
	void setHeuristic(int);

	void setIndices(int, int);

	const PathfindingAlgorithm<Vec2, float>& getCurrentAlgorithm() const;
	const Heuristic<Vec2, float>& getCurrentHeuristic() const;

private:
	bool m_showSettingsDialog = false;
	int m_startIndex = 0;
	int m_goalIndex = 0;
	OutputMessage m_groupMessage;

	std::vector<std::pair<Heuristic<Vec2,float>, std::string>> m_heuristics;
	int m_heuristicIndex = 0;

	std::vector<std::pair<PathfindingAlgorithm<Vec2,float>, std::string>> m_algorithms;
	int m_algorithmIndex = 1;

	bool m_showProfilingDialog = false;
	int m_profilerIterations = 100;
	bool m_profilerBlocking = false;
	std::unique_ptr<Profiler> m_profiler = nullptr;
	OutputMessage m_profilerMessage;

	void finalProfilerMessage();
	void checkOnProfiling();
};