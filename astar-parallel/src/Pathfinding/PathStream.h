#pragma once

#include <string>
#include <sstream>
#include "Prototypes.h"

std::string pathOut(const Path& path, const char* delim = ", ") {
	std::stringstream ss;
	for (int i = 0; i < path.size(); ++i) {
		ss << path.at(i);
		if ((i + 1) < path.size()) { ss << delim; }
	}
	return ss.str();
}

template<typename Value, typename Weight>
std::string pathOut(const Path& path, const DirectedGraph<Value, Weight>& graph, const char* delim = ", ") {
	std::stringstream ss;
	for (int i = 0; i < path.size(); ++i) {
		ss << path.at(i) << "(" << graph.at(path.at(i)).value() << ")";
		if (i + 1 < path.size()) { ss << delim; }
	}
	return ss.str();
}