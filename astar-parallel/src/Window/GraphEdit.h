#pragma once

#include "../Maths/Vec2.h"
#include "../Graph/DirectedGraph.h"
#include <functional>

#include "../Pathfinding/Prototypes.h"
#include "ImGuiUtil.h"

class GraphEdit
{
public:
	GraphEdit();

	void addMenuBarItem();
	void imguiDrawWindow(int width, int height);

	void imguiDisplayGraph();

	void saveGraph(std::string path);
	void loadGraph(std::string path);
	void generateGraph();

private:
	bool m_show = false;

	int m_setNode_index = 0;
	float m_setNode_inputPosition[2] = { 0.f,0.f };
	OutputMessage m_setNode_message;

	int m_setEdge_startIndex = 0;
	int m_setEdge_endIndex = 0;
	bool m_setEdge_doubleEdge = true;
	OutputMessage m_setEdge_message;

	bool m_showSaveLoadDialog = false;
	bool m_saveLoadDialogIsSave = true;
	char m_inputSavePath[256];
	OutputMessage m_saveLoadMessage;

	bool m_showGraphAdjacencyTable = false;


	bool m_showGenerateDialog = false;
	int m_generate_numNodes = 30;
	int m_generate_k = 5;
	bool m_generate_doubleEdged = false;
	float m_generate_lowerBound[2] = { -100.f, -100.f };
	float m_generate_upperBound[2] = { 100.f, 100.f };

	std::vector<std::pair<Heuristic<Vec2,float>, std::string>> m_heuristics;
	int m_heuristicIndex = 0;
};