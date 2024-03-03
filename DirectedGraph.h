#pragma once
#include <initializer_list>
#include <vector>
#include <map>
#include <stdexcept>

template<class ValueType, class WeightType = int>
class DirectedGraph
{
public:
	struct Node {
		ValueType value;
		std::map<int, WeightType> adjacencyMap;
		Node(const ValueType& value) : value(value) {}
	};

	struct Edge {
		int start, end; WeightType weight;
		Edge(int start, int end, WeightType weight) : start(start), end(end), weight(weight) {}
	};

	DirectedGraph(std::initializer_list<ValueType> values, std::initializer_list<Edge> edges) {
		int nodeCount = values.size();
		m_nodes.reserve(nodeCount);
		for (const ValueType& value : values) { m_nodes.push_back(Node(value)); }

		for (const Edge& edge : edges) {
			if (edge.start < 0 || edge.start >= nodeCount || edge.end < 0 || edge.end >= nodeCount) { throw std::out_of_range("Edge contains invalid indices."); }
			m_nodes[edge.start].adjacencyMap.insert_or_assign(edge.end, edge.weight);
		}
	}

	const Node& at(int index) const { return m_nodes.at(index); }

	int nodeCount() const { return m_nodes.size(); }

private:
	std::vector<Node> m_nodes;
};