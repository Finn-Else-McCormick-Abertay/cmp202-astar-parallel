#pragma once

#include <string>
#include <sstream>

template<typename ...Args> std::string stringOut(Args... args) {
	std::stringstream ss;
	(ss << ... << args);
	return ss.str();
}