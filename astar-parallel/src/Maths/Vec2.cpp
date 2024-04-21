#include "Vec2.h"

#include <cmath>

float Vec2::length() const {
	return sqrtf(powf(x, 2.f) + powf(y, 2.f));
}


Vec2 Vec2::operator+(const Vec2& other) const {
	return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const {
	return Vec2(x - other.x, y - other.y);
}


std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	return os << "[" << v.x << ", " << v.y << "]";
}