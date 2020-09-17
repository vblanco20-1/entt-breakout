#pragma once
#include <math.h>
#include <random>

constexpr double DEG_2_RAD = 0.01745329252;
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
    double lenghtd() const {
        return sqrt(x * x + y * y);
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

	void rotate_degrees(double angleDegrees) {
		
		//double len = lenghtd();
		//double nx = (double)x / len;
		//double ny = (double)y / len;
		double anglerad = angleDegrees * DEG_2_RAD;

        double nx = x * cos(anglerad) - y * sin(anglerad);
        double ny = x * sin(anglerad) + y * cos(anglerad);
        //double angle = atan(ny / nx);
        //angle += anglerad;
        //
        //nx = cos(angle) * len;
        //ny = sin(angle) * len;

		x = nx;
		y = ny;
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

