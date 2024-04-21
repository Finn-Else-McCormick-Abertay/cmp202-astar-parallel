#pragma once

#include <imgui.h>
#include <extensions/implot/implot.h>

#include <functional>
#include <string>

#include "DirectedGraph.h"

struct NodeDisplayInfo
{
	std::string name; ImVec2 pos;
	NodeDisplayInfo(std::string name, ImVec2 pos) : name(name), pos(pos) {}
	NodeDisplayInfo() = default;
};

template<class ValueType, class WeightType = int>
void DisplayGraph(const DirectedGraph<ValueType, WeightType>& graph, const std::vector<int>& path, const std::function<NodeDisplayInfo(const ValueType&, const int&)>& GetNodeInfo) {
	
	NodeDisplayInfo* info = new NodeDisplayInfo[graph.nodeCount()];
	for (int i = 0; i < graph.nodeCount(); ++i) {
		info[i] = GetNodeInfo(graph.at(i).value(), i);
	}

	if (ImGui::BeginTable("##adjacencyMatrix", graph.nodeCount() + 1)) {
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort);
		for (int i = 0; i < graph.nodeCount(); ++i) {
			auto& nodeInfo = info[i];
			ImGui::TableSetupColumn(nodeInfo.name.c_str(), ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort);
		}
		ImGui::TableHeadersRow();

		for (int row = 0; row < graph.nodeCount(); ++row) {
			auto& rowAdjacencyMap = graph.at(row).adjacencyMap();
			auto& rowInfo = info[row];
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader(rowInfo.name.c_str());
			for (int column = 0; column < graph.nodeCount(); ++column) {
				ImGui::TableSetColumnIndex(column + 1);
				if (rowAdjacencyMap.contains(column)) {
					WeightType weight = rowAdjacencyMap.at(column);
					ImGui::Text("%s", std::to_string(weight).c_str());
				}
			}
		}
		ImGui::EndTable();
	}

	ImColor color_labels = ImColor(0.f, 0.f, 0.f, 1.f);
	ImColor color_lines_base = ImColor(0.7f, 0.7f, 0.7f, 1.f);
	ImColor color_lines_path = ImColor(1.f, 0.2f, 0.2f, 1.f);

	if (ImPlot::BeginPlot("##Graph", ImVec2(-1, -1), ImPlotFlags_NoFrame)) {

		ImPlot::SetupAxes(NULL, NULL, 0, 0);
		ImPlot::SetupAxesLimits(-10, 10, -10, 10);

		for (int startIndex = 0; startIndex < graph.nodeCount(); ++startIndex) {
			auto& startInfo = info[startIndex];
			auto& adjacencyMap = graph.at(startIndex).adjacencyMap();

			for (auto [endIndex, weight] : adjacencyMap) {
				auto& endInfo = info[endIndex];

				float xs[2] = { startInfo.pos.x, endInfo.pos.x };
				float ys[2] = { startInfo.pos.y, endInfo.pos.y };

				ImPlot::SetNextLineStyle(color_lines_base, 1.f);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);

				/*
				ImPlot::PlotText(std::to_string(weight).c_str(),
					startInfo.pos.x + ((endInfo.pos.x - startInfo.pos.x) * 0.3f),
					startInfo.pos.y + ((endInfo.pos.y - startInfo.pos.y) * 0.3f)
				);
				*/
			}
		}
		if (path.size() > 0) {
			for (int i = 0; i < path.size() - 1; ++i) {
				int startIndex = path.at(i);
				int endIndex = path.at(i + 1);

				auto& startInfo = info[startIndex];
				auto& endInfo = info[endIndex];

				float xs[2] = { startInfo.pos.x, endInfo.pos.x };
				float ys[2] = { startInfo.pos.y, endInfo.pos.y };

				ImPlot::SetNextLineStyle(color_lines_path, 1.5f);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);
			}
		}

		ImVec2 textOffset{2, 2};
		ImPlot::PushStyleColor(ImPlotCol_InlayText, ImU32(color_labels));
		for (int i = 0; i < graph.nodeCount(); ++i) {
			auto& nodeInfo = info[i];
			ImPlot::PlotText(nodeInfo.name.c_str(), nodeInfo.pos.x, nodeInfo.pos.y, textOffset);
		}
		ImPlot::PopStyleColor();

		ImPlot::EndPlot();
	}

	delete[] info;
}