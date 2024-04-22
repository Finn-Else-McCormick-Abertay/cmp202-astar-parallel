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


Vec2 operator*(const Vec2& v, const float& s) {
	return Vec2(v.x * s, v.y * s);
}
Vec2 operator*(const float& s, const Vec2& v) {
	return Vec2(v.x * s, v.y * s);
}

Vec2 Vec2::operator/(const float& s) const {
	return Vec2(x / s, y / s);
}

float Vec2::dot(const Vec2& other) const {
	return x * other.x + y * other.y;
}

Vec2 Vec2::normalized() const {
	return (*this) / length();
}


ImVec2 Vec2::asImVec2() const { return ImVec2(x, y); }


std::ostream& operator<<(std::ostream& os, const Vec2& v) {
	return os << "[" << v.x << ", " << v.y << "]";
}