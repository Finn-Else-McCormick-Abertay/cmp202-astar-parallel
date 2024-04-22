#pragma once

#include <imgui.h>
#include <extensions/implot/implot.h>

#include <functional>
#include <string>
#include "../Maths/Vec2.h"
#include <numbers>

#include "DirectedGraph.h"

struct NodeDisplayInfo
{
	std::string name; ImVec2 pos;
	NodeDisplayInfo(std::string name, ImVec2 pos) : name(name), pos(pos) {}
	NodeDisplayInfo() = default;
};

template<class ValueType, class WeightType = int>
void DisplayGraph(const DirectedGraph<ValueType, WeightType>& graph, const std::vector<int>& path, const std::function<NodeDisplayInfo(const ValueType&, const int&)>& GetNodeInfo) {
	
	NodeDisplayInfo* info = new NodeDisplayInfo[graph.size()];

	for (int i = 0; i < graph.size(); ++i) {
		info[i] = GetNodeInfo(graph.at(i).value(), i);
	}

	if (ImGui::BeginTable("##adjacencyMatrix", graph.size() + 1)) {
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort);
		for (int i = 0; i < graph.size(); ++i) {
			auto& nodeInfo = info[i];
			ImGui::TableSetupColumn(nodeInfo.name.c_str(), ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoSort);
		}
		ImGui::TableHeadersRow();

		for (int row = 0; row < graph.size(); ++row) {
			auto& rowAdjacencyMap = graph.at(row).adjacencyMap();
			auto& rowInfo = info[row];
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::TableHeader(rowInfo.name.c_str());
			for (int column = 0; column < graph.size(); ++column) {
				ImGui::TableSetColumnIndex(column + 1);
				if (rowAdjacencyMap.contains(column)) {
					WeightType weight = rowAdjacencyMap.at(column);
					ImGui::Text("%s", std::to_string(weight).c_str());
				}
			}
		}
		ImGui::EndTable();
	}

	ImColor color_labels = ImColor(1.f, 1.f, 1.f, 1.f);
	ImColor color_points = ImColor(0.5f, 0.5f, 0.5f, 1.f);
	ImColor color_lines_base = ImColor(0.7f, 0.7f, 0.7f, 1.f);
	ImColor color_lines_path = ImColor(1.f, 0.2f, 0.2f, 1.f);

	float arrowLinePosition = 0.7f;
	float arrowSize = 6.f;

	auto drawArrow = [arrowLinePosition, arrowSize](ImVec2 start, ImVec2 end, ImColor color) {
		Vec2 startPoint = Vec2(ImPlot::PlotToPixels(start.x, start.y)), endPoint = Vec2(ImPlot::PlotToPixels(end.x, end.y));

		Vec2 drawPoint = startPoint + arrowLinePosition * (endPoint - startPoint);

		Vec2 upVec = Vec2(0.f, 1.f).normalized();
		Vec2 lineVec = (startPoint - endPoint).normalized();
		float dot = upVec.dot(lineVec);
		float determinant = upVec.x * lineVec.y - upVec.y * lineVec.x;
		float lineHeadingAngle = atan2f(determinant, dot);

		auto rotate = [](const Vec2& v, float angle) {
			float cosA = cosf(angle), sinA = sinf(angle);
			return Vec2(
				v.x * cosA - v.y * sinA,
				v.x * sinA + v.y * cosA
			);
			};

		Vec2 arrow_p1 = drawPoint + rotate(Vec2(arrowSize / 2.f, arrowSize / 2.f), lineHeadingAngle);
		Vec2 arrow_p2 = drawPoint + rotate(Vec2(0.f, -arrowSize), lineHeadingAngle);
		Vec2 arrow_p3 = drawPoint + rotate(Vec2(-arrowSize / 2.f, arrowSize / 2.f), lineHeadingAngle);

		ImDrawList* drawList = ImPlot::GetPlotDrawList();
		drawList->AddTriangleFilled(arrow_p1.asImVec2(), arrow_p2.asImVec2(), arrow_p3.asImVec2(), color);
		};

	if (ImPlot::BeginPlot("##Graph", ImVec2(-1, -1), ImPlotFlags_NoFrame)) {

		ImPlot::SetupAxes(NULL, NULL, 0, 0);
		ImPlot::SetupAxesLimits(-10, 10, -10, 10);

		for (int startIndex = 0; startIndex < graph.size(); ++startIndex) {
			auto& startInfo = info[startIndex];
			auto& adjacencyMap = graph.at(startIndex).adjacencyMap();

			for (auto [endIndex, weight] : adjacencyMap) {
				auto& endInfo = info[endIndex];

				float xs[2] = { startInfo.pos.x, endInfo.pos.x };
				float ys[2] = { startInfo.pos.y, endInfo.pos.y };

				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 8.f, color_points);
				ImPlot::SetNextLineStyle(color_lines_base, 1.f);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);

				drawArrow(startInfo.pos, endInfo.pos, color_points);
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

				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, 8.f, color_lines_path);
				ImPlot::SetNextLineStyle(color_lines_path, 1.5f);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);

				drawArrow(startInfo.pos, endInfo.pos, color_lines_path);
			}
		}

		ImVec2 textOffset{1, 0};
		ImPlot::PushStyleColor(ImPlotCol_InlayText, ImU32(color_labels));
		for (int i = 0; i < graph.size(); ++i) {
			auto& nodeInfo = info[i];
			ImPlot::PlotText(nodeInfo.name.c_str(), nodeInfo.pos.x, nodeInfo.pos.y, textOffset);
		}
		ImPlot::PopStyleColor();

		ImPlot::EndPlot();
	}

	delete[] info;
}