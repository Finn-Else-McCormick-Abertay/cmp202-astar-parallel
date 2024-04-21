#pragma once

#include <ostream>

class Vec2
{
public:
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2() = default;
	Vec2(const Vec2&) = default;

	float x, y;

	float length() const;

	Vec2 operator+(const Vec2&) const;
	Vec2 operator-(const Vec2&) const;

	friend std::ostream& operator<<(std::ostream& os, const Vec2& v);
};

std::ostream& operator<<(std::ostream& os, const Vec2& v);