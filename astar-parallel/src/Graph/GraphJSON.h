#pragma once

#include "../../nlohmann/json.hpp"
#include "../Maths/Vec2.h"
#include "DirectedGraph.h"

#include <fstream>
#include <filesystem>

using json = nlohmann::json;

void to_json(json& j, const Vec2& v);
void from_json(const json& j, Vec2& v);

template<class ValueType, class WeightType>
void to_json(json& j, const DirectedGraph<ValueType, WeightType>& graph) {
	for (int i = 0; i < graph.size(); ++i) {
		json node;
		node["index"] = i;
		node["value"] = graph.at(i).value();
		json edges;
		const std::map<int, WeightType>& map = graph.at(i).adjacencyMap();
		for (auto& [index, weight] : map) {
			json edge;
			edge["index"] = index;
			edge["weight"] = weight;
			edges.push_back(edge);
		}
		node["edges"] = edges;
		j.push_back(node);
	}
}

template<class ValueType, class WeightType>
void from_json(const json& j, DirectedGraph<ValueType, WeightType>& graph) {
	for (const json& node : j) {
		ValueType value = node.at("value").get<ValueType>();
		graph.createNode(value);
	}
	for (const json& node : j) {
		int index = node.at("index").get<int>();
		const json& edges = node.at("edges");
		for (const json& edge : edges) {
			int neighbourIndex = edge.at("index").get<int>();
			WeightType weight = edge.at("weight").get<WeightType>();
			graph.setEdgeWeight(index, neighbourIndex, weight);
		}
	}
}


template<class ValueType, class WeightType>
bool saveToFile(const DirectedGraph<ValueType, WeightType>& graph, std::string path) {
	std::filesystem::path filepath = std::filesystem::path(path).replace_extension("json");
	std::ofstream file(filepath);
	if (file.is_open()) {
		json data = graph;
		file << data;
		return true;
	}
	return false;
}

template<class ValueType, class WeightType>
std::pair<DirectedGraph<ValueType, WeightType>, bool> loadFromFile(std::string path) {
	std::filesystem::path filepath = std::filesystem::path(path).replace_extension("json");
	std::ifstream file(filepath);
	if (file.is_open()) {
		json data = json::parse(file);
		return std::make_pair(data.get<DirectedGraph<ValueType, WeightType>>(), true);
	}
	return std::make_pair(DirectedGraph<ValueType, WeightType>(), false);
}