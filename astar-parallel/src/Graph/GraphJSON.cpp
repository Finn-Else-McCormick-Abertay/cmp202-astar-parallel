#include "GraphJSON.h"

void to_json(json& j, const Vec2& v) {
	j = json{ {"x",v.x}, {"y", v.y} };
}

void from_json(const json& j, Vec2& v) {
	j.at("x").get_to(v.x);
	j.at("y").get_to(v.y);
}