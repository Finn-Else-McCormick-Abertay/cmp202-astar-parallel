#pragma once
#include <initializer_list>
#include <vector>
#include <map>
#include <stdexcept>

template<class ValueType, class WeightType = int>
class DirectedGraph
{
public:
	class Node {
	public:
		Node(const ValueType& value) : m_value(value) {}
		const ValueType& value() const { return m_value; }
		const std::map<int, WeightType>& adjacencyMap() const { return m_adjacencyMap; }

		void setValue(ValueType val) { m_value = val; }
		void setEdgeWeight(int index, WeightType weight) { m_adjacencyMap.insert_or_assign(index, weight); }
		void removeEdge(int index) { m_adjacencyMap.erase(index); }
	private:
		ValueType m_value;
		std::map<int, WeightType> m_adjacencyMap;
	};

	struct Edge {
		int start, end; WeightType weight;
		Edge(int start, int end, WeightType weight = static_cast<WeightType>(1)) : start(start), end(end), weight(weight) {}
	};

	DirectedGraph() = default;
	DirectedGraph(std::initializer_list<ValueType> values, std::initializer_list<Edge> edges, bool twoWayEdges = false) {
		m_nodes.reserve(values.size()); for (const ValueType& value : values) { createNode(value); }
		for (const Edge& edge : edges) { setEdgeWeight(edge.start, edge.end, edge.weight, twoWayEdges); }
	}

	const Node& at(int index) const { return m_nodes.at(index); }
	bool has(int index) const { return index >= 0 && index < m_nodes.size(); }

	size_t size() const { return m_nodes.size(); }

	int createNode(ValueType val) { m_nodes.push_back(Node(val)); return m_nodes.size(); }
	void setValue(int index, ValueType val) { m_nodes[index].setValue(val); }
	void setEdgeWeight(int start, int end, WeightType weight, bool twoWay = false) {
		if (start < 0 || start >= size() || end < 0 || end >= size()) { throw std::out_of_range("Edge contains invalid indices."); }
		m_nodes[start].setEdgeWeight(end, weight);
		if (twoWay) { m_nodes[end].setEdgeWeight(start, weight); }
	}
	void removeEdge(int start, int end, bool twoWay = false) {
		if (start < 0 || start >= size() || end < 0 || end >= size()) { throw std::out_of_range("Edge contains invalid indices."); }
		m_nodes[start].removeEdge(end);
		if (twoWay) { m_nodes[end].removeEdge(start); }
	}

private:
	std::vector<Node> m_nodes;
};