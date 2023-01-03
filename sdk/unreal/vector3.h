#pragma once

#include <sstream>
#include "../includes.h"

struct Vector3
{

	double x, y, z;

	bool IsValid() const
	{
#define valid(x) x > -999 && x < 999999999999999999.f
		return valid(x) && valid(x) && valid(x);
	}

	float Dot(Vector3 other)
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	std::string Print()
	{

		std::stringstream ss;
		ss << "X : " << x << " Y : " << y << " Z : " << z;
		return ss.str();

	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(float number) const {
		return Vector3(x * number, y * number, z * number);
	}

};

struct Vector2
{
	float Distance(Vector2 b)
	{
		return sqrt(pow(b.x - x, 2) + pow(b.y - y, 2));
	}

	double x, y;
};

struct FRotator
{
	double Pitch, Yaw, Roll;
};