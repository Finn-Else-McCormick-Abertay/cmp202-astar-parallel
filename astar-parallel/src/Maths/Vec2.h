#pragma once

#include <ostream>
#include <imgui.h>

class Vec2
{
public:
	Vec2(float x, float y) : x(x), y(y) {}
	Vec2() = default;
	Vec2(const Vec2&) = default;
	Vec2(const ImVec2& v) : Vec2(v.x, v.y) {}

	float x, y;

	float length() const;

	Vec2 operator+(const Vec2&) const;
	Vec2 operator-(const Vec2&) const;

	friend Vec2 operator*(const Vec2& lhs, const float& rhs);
	friend Vec2 operator*(const float& lhs, const Vec2& rhs);

	Vec2 operator/(const float& rhs) const;

	float dot(const Vec2&) const;

	Vec2 normalized() const;

	ImVec2 asImVec2() const;

	friend std::ostream& operator<<(std::ostream& os, const Vec2& v);
};

Vec2 operator*(const Vec2& lhs, const float& rhs);
Vec2 operator*(const float& lhs, const Vec2& rhs);

std::ostream& operator<<(std::ostream& os, const Vec2& v);