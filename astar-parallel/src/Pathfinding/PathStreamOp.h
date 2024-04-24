#pragma once

#include <ostream>
#include "Prototypes.h"

void pathOutput(std::ostream& os, const Path& path, const char* delim = ", ") {
	for (int i = 0; i < path.size(); ++i) {
		os << path.at(i);
		if ((i + 1) < path.size()) { os << delim; }
	}
}

template<typename Value, typename Weight>
void pathOutput(std::ostream& os, const Path& path, const DirectedGraph<Value, Weight>& graph, const char* delim = ", ") {
	for (int i = 0; i < path.size(); ++i) {
		os << path.at(i) << "(" << graph.at(path.at(i)).value() << ")";
		if (i + 1 < path.size()) { os << delim; }
	}
}