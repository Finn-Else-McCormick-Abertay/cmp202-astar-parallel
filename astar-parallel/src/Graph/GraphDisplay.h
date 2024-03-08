#pragma once

#include <imgui.h>
#include <extensions/implot/implot.h>

#include <functional>

#include "DirectedGraph.h"

template<class ValueType, class WeightType = int>
void DisplayGraph(const DirectedGraph<ValueType, WeightType>& graph, std::function<ImVec2(ValueType, int)> const& ValueToPosition) {
	if (ImPlot::BeginPlot("##Graph", ImVec2(-1, 0), ImPlotFlags_CanvasOnly)) {

		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
		ImPlot::SetupAxesLimits(0, 10, 0, 12);

		for (int startIndex = 0; startIndex < graph.nodeCount(); ++startIndex) {
			auto& startNode = graph.at(startIndex);
			ImVec2 startPos = ValueToPosition(startNode.value, startIndex);
			auto& map = startNode.adjacencyMap;
			for (auto [endIndex, weight] : map) {
				auto& endNode = graph.at(endIndex);
				ImVec2 endPos = ValueToPosition(endNode.value, endIndex);

				float xs[2] = { startPos.x, endPos.x };
				float ys[2] = { startPos.y, endPos.y };

				ImPlot::PlotLine("##GraphLine", xs, ys, 2);
			}

			//ImGui::PushID(m);
			//ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 1.f, IMPLOT_AUTO_COL, 1.f);
			//ImPlot::PlotLine("##GraphLine", xs, ys, 2);
			//ImGui::PopID();
		}

		//ImPlot::PlotText("Filled Markers", 2.5f, 6.0f);
		//ImPlot::PlotText("Open Markers", 7.5f, 6.0f);

		//ImPlot::PushStyleColor(ImPlotCol_InlayText, ImVec4(1, 0, 1, 1));
		//ImPlot::PlotText("Vertical Text", 5.0f, 6.0f, ImVec2(0, 0), ImPlotTextFlags_Vertical);
		//ImPlot::PopStyleColor();

		ImPlot::EndPlot();
	}
}