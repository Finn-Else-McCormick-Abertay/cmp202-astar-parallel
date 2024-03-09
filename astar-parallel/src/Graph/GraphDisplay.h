#pragma once

#include <imgui.h>
#include <extensions/implot/implot.h>

#include <functional>
#include <string>

#include "DirectedGraph.h"

struct NodeDisplayInfo
{
	std::string name; ImVec2 pos; ImColor color;
	NodeDisplayInfo(std::string name, ImVec2 pos, ImColor color = IM_COL32_BLACK) : name(name), pos(pos), color(color) {}
	NodeDisplayInfo() = default;
};

template<class ValueType, class WeightType = int>
void DisplayGraph(const DirectedGraph<ValueType, WeightType>& graph, std::function<NodeDisplayInfo(ValueType, int)> const& GetNodeInfo) {
	
	NodeDisplayInfo* info = new NodeDisplayInfo[graph.nodeCount()];
	for (int i = 0; i < graph.nodeCount(); ++i) {
		auto& node = graph.at(i);
		info[i] = GetNodeInfo(node.value, i);
	}

	if (ImGui::BeginTable("##adjacencyMatrix", graph.nodeCount() + 1)) {
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort);
		for (int i = 0; i < graph.nodeCount(); ++i) {
			auto& nodeInfo = info[i];
			ImGui::TableSetupColumn(nodeInfo.name.c_str(), ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort);
		}
		ImGui::TableHeadersRow();

		for (int row = 0; row < graph.nodeCount(); ++row) {
			auto& rowNode = graph.at(row);
			auto& rowInfo = info[row];
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader(rowInfo.name.c_str());
			for (int column = 0; column < graph.nodeCount(); ++column) {
				ImGui::TableSetColumnIndex(column + 1);
				if (rowNode.adjacencyMap.contains(column)) {
					WeightType weight = rowNode.adjacencyMap.at(column);
					ImGui::Text("%s", std::to_string(weight).c_str());
				}
			}
		}
		ImGui::EndTable();
	}

	if (ImPlot::BeginPlot("##Graph", ImVec2(-1, -1), ImPlotFlags_NoFrame)) {

		ImPlot::SetupAxes(NULL, NULL, ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations);
		ImPlot::SetupAxesLimits(-10, 10, -10, 10);

		for (int startIndex = 0; startIndex < graph.nodeCount(); ++startIndex) {
			auto& startNode = graph.at(startIndex);
			auto& startInfo = info[startIndex];

			ImPlot::PushStyleColor(ImPlotCol_Line, ImU32(startInfo.color));
			for (auto [endIndex, weight] : startNode.adjacencyMap) {
				auto& endInfo = info[endIndex];

				float xs[2] = { startInfo.pos.x, endInfo.pos.x };
				float ys[2] = { startInfo.pos.y, endInfo.pos.y };

				ImPlot::SetNextLineStyle(startInfo.color, weight);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);

				ImPlot::PlotText(std::to_string(weight).c_str(),
					startInfo.pos.x + ((endInfo.pos.x - startInfo.pos.x) * 0.3f),
					startInfo.pos.y + ((endInfo.pos.y - startInfo.pos.y) * 0.3f)
				);
			}

			ImPlot::PopStyleColor();
		}

		ImVec2 textOffset{2, 2};
		for (int i = 0; i < graph.nodeCount(); ++i) {
			auto& nodeInfo = info[i];
			ImPlot::PushStyleColor(ImPlotCol_InlayText, ImU32(nodeInfo.color));
			ImPlot::PlotText(nodeInfo.name.c_str(), nodeInfo.pos.x, nodeInfo.pos.y, textOffset);
			ImPlot::PopStyleColor();
		}

		ImPlot::EndPlot();
	}

	delete[] info;
}