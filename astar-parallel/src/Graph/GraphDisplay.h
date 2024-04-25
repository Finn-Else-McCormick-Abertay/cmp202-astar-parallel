#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <extensions/implot/implot.h>

#include <functional>
#include <string>
#include "../Maths/Vec2.h"
#include <numbers>

#include "DirectedGraph.h"

#include "../Singleton.h"
#include "../StringUtil.h"

struct NodeDisplayInfo
{
	std::string name; ImVec2 pos;
	NodeDisplayInfo(std::string name, ImVec2 pos) : name(name), pos(pos) {}
	NodeDisplayInfo() = default;
};

template<class ValueType, class WeightType = int>
void DisplayAdjacencyTable(const DirectedGraph<ValueType, WeightType>& graph, const std::vector<int>& path, const std::function<NodeDisplayInfo(const ValueType&, const int&)>& GetNodeInfo) {

	int* heatmapValues = new int[graph.size() * graph.size()];
	for (int i = 0; i < graph.size(); ++i) { for (int j = 0; j < graph.size(); ++j) { heatmapValues[i + j * graph.size()] = 0; } }

	for (int i = 0; i < graph.size(); ++i) {
		NodeDisplayInfo info = GetNodeInfo(graph.at(i).value(), i);
		auto adjacencyMap = graph.at(i).adjacencyMap();
		for (auto& [neighbour, edgeWeight] : adjacencyMap) {
			// Round and store as an integer as this displays better in printf formatting
			// (both in that it's shorter and in that it lets us display nothing for zeros)
			heatmapValues[(graph.size() - 1 - i) * graph.size() + neighbour] = static_cast<int>(round(edgeWeight));
		}
	}


	static ImU32 colormapValues[] = { ImColor(1.f,1.f,1.f), ImColor(0.5f,0.5f,0.5f) };
	static ImPlotColormap colormap = ImPlot::AddColormap("##adjacencyColormap", colormapValues, IM_ARRAYSIZE(colormapValues), false);

	ImPlot::PushColormap(colormap);
	
	if (ImPlot::BeginPlot("##adjacencyMatrix", ImVec2(-1, 0), ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText | ImPlotFlags_NoFrame)) {
		ImPlotAxisFlags axisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoHighlight;
		ImPlot::SetupAxes(NULL,NULL, axisFlags | ImPlotAxisFlags_Opposite, axisFlags | ImPlotAxisFlags_Invert);
		ImPlotPoint boundsMin = ImPlotPoint(0, 0), boundsMax = ImPlotPoint(graph.size(), graph.size());
		ImPlot::SetupAxesLimits(boundsMin.x, boundsMax.x, boundsMin.y, boundsMax.y);
		ImPlot::SetupAxisFormat(ImAxis_X1, "%.f");
		ImPlot::SetupAxisFormat(ImAxis_Y1, "%.f");
		ImPlot::SetupAxisTicks(ImAxis_X1, boundsMin.x, boundsMax.x, graph.size());
		ImPlot::SetupAxisTicks(ImAxis_Y1, boundsMin.y, boundsMax.y, graph.size());
		ImPlot::PlotHeatmap("##adjHeatmap", heatmapValues, graph.size(), graph.size(), 0.f, 100.f, "%.i", boundsMin, boundsMax);
		ImPlot::EndPlot();
	}

	ImPlot::PopColormap();

	delete[] heatmapValues;
}

template<class ValueType, class WeightType = int>
void DisplayGraph(const DirectedGraph<ValueType, WeightType>& graph, const std::vector<int>& path, const std::function<NodeDisplayInfo(const ValueType&, const int&)>& GetNodeInfo) {
	
	NodeDisplayInfo* info = new NodeDisplayInfo[graph.size()];

	for (int i = 0; i < graph.size(); ++i) {
		info[i] = GetNodeInfo(graph.at(i).value(), i);
	}

	ImColor color_labels = ImColor(1.f, 1.f, 1.f, 1.f);
	ImColor color_points = ImColor(0.5f, 0.5f, 0.5f, 1.f);
	ImColor color_lines_base = ImColor(0.7f, 0.7f, 0.7f, 1.f);
	ImColor color_lines_path = ImColor(1.f, 0.2f, 0.2f, 1.f);

	float arrowLinePosition = 0.7f;

	float lineThickness = 1.5f; float pointSize = 8.f;
	float pathLineThickness = 2.5f; float pathPointSize = 10.f;
	float arrowSize = 6.f; float pathArrowSize = 7.5f;

	auto drawArrow = [arrowLinePosition, arrowSize](ImVec2 start, ImVec2 end, ImColor color, float size) {
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

		Vec2 arrow_p1 = drawPoint + rotate(Vec2(size / 2.f, size / 2.f), lineHeadingAngle);
		Vec2 arrow_p2 = drawPoint + rotate(Vec2(0.f, -size), lineHeadingAngle);
		Vec2 arrow_p3 = drawPoint + rotate(Vec2(-size / 2.f, size / 2.f), lineHeadingAngle);

		ImVec2 plotOrigin = ImPlot::GetPlotPos();
		ImVec2 plotSize = ImPlot::GetPlotSize();
		auto withinLimits = [&plotOrigin, &plotSize](const Vec2& p) {
			return (p.x > plotOrigin.x) && (p.x < (plotOrigin.x + plotSize.x)) && (p.y > plotOrigin.y) && (p.y < (plotOrigin.y + plotSize.y));
		};

		if (withinLimits(arrow_p1) && withinLimits(arrow_p2) && withinLimits(arrow_p3)) {
			ImDrawList* drawList = ImPlot::GetPlotDrawList();
			drawList->AddTriangleFilled(arrow_p1.asImVec2(), arrow_p2.asImVec2(), arrow_p3.asImVec2(), color);
		}
	};

	if (ImPlot::BeginPlot("##GraphDisplayMain", ImVec2(-1, -1), ImPlotFlags_NoFrame)) {

		ImPlot::SetupAxes(NULL, NULL, 0, 0);
		ImPlot::SetupAxesLimits(-100, 100, -100, 100);

		for (int startIndex = 0; startIndex < graph.size(); ++startIndex) {
			auto& startInfo = info[startIndex];
			auto& adjacencyMap = graph.at(startIndex).adjacencyMap();

			for (auto [endIndex, weight] : adjacencyMap) {
				auto& endInfo = info[endIndex];

				float xs[2] = { startInfo.pos.x, endInfo.pos.x };
				float ys[2] = { startInfo.pos.y, endInfo.pos.y };

				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, pointSize, color_points);
				ImPlot::SetNextLineStyle(color_lines_base, lineThickness);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);

				drawArrow(startInfo.pos, endInfo.pos, color_lines_base, arrowSize);
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

				ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle, pathPointSize, color_lines_path);
				ImPlot::SetNextLineStyle(color_lines_path, pathLineThickness);
				ImPlot::PlotLine("##GraphLine", xs, ys, 2);

				drawArrow(startInfo.pos, endInfo.pos, color_lines_path, pathArrowSize);
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