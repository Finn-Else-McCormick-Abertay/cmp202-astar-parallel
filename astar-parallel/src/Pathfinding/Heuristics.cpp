#include "Heuristics.h"

float straightLineDistance(const Vec2& v1, const Vec2& v2) { return (v1 - v2).length(); }

float manhattanDistance(const Vec2& v1, const Vec2& v2) { return abs(v1.x - v2.x) + abs(v1.y - v2.y); }