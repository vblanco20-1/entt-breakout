#pragma once
#include <math.h>
#include <random>

struct Vec2f {
	float x{ 0.f };
	float y{ 0.f };

	constexpr Vec2f operator+(const Vec2f& other)const
	{
		Vec2f v{};
		v.x = x + other.x;
		v.y = y + other.y;
		return v;
	}
	constexpr Vec2f operator-(const Vec2f& other)const
	{
		Vec2f v{};
		v.x = x - other.x;
		v.y = y - other.y;
		return v;
	}
	Vec2f &operator+=(const Vec2f& other)
	{
		x = x + other.x;
		y = y + other.y;
		return *this;
	}
	Vec2f &operator-=(const Vec2f& other)
	{
		x = x - other.x;
		y = y - other.y;
		return *this;
	}

	float lenght() const {
		return sqrt(x*x + y * y);
	}

	Vec2f normalized() {
		float l = lenght();

		return Vec2f{ x / l, y / l };
	}
	constexpr Vec2f operator*(float other)const
	{
		Vec2f v{};
		v.x = x * other;
		v.y = y * other;
		return v;
	}
};

struct Vec2i {

	int x{ 0 };
	int y{ 0 };

	constexpr Vec2i operator+(const Vec2i& other)const
	{
		Vec2i v{};
		v.x = x + other.x;
		v.y = y + other.y;
		return v;
	}

	constexpr Vec2i operator-(const Vec2i& other)const
	{
		Vec2i v{};
		v.x = x - other.x;
		v.y = y - other.y;
		return v;
	}
	Vec2i &operator+=(const Vec2i& other)
	{
		x = x + other.x;
		y = y + other.y;
		return *this;
	}
	Vec2i &operator-=(const Vec2i& other)
	{
		x = x - other.x;
		y = y - other.y;
		return *this;
	}
	constexpr Vec2i operator*(float other)const
	{
		Vec2i v{};
		v.x = int(float(x) * other);
		v.y = int(float(y) * other);
		return v;
	}
};

Vec2f random_vector();

