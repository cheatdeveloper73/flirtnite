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

	double AngleTo(Vector3 source, Vector3 dest)
	{
		if (source == dest) return 0.f;
		double dot = Dot(dest);
		return acos(dot);
	}

	FORCEINLINE Vector3 GetSafeNormal() const
	{	
		float SquareSum = x * x + y * y + z * z;
		const double Scale = 1.0/sqrt(SquareSum);
		return Vector3(x * Scale, y * Scale, z * Scale);
	}

	void VectorCrossProduct(const Vector3& a, const Vector3& b, Vector3& result)
	{
		result.x = a.y * b.z - a.z * b.y;
		result.y = a.z * b.x - a.x * b.z;
		result.z = a.x * b.y - a.y * b.x;
	}

	Vector3 Cross(const Vector3& vOther)
	{
		Vector3 res;
		VectorCrossProduct(*this, vOther, res);

		return res;
	}

	Vector3 Clamp()
	{
		if (x < -89.0f)
			x = -89.0f;

		if (x > 89.0f)
			x = 89.0f;

		while (y < -180.0f)
			y += 360.0f;

		while (y > 180.0f)
			y -= 360.0f;

		z = 0.0f;
		return *this;
	}

	float Length2D() const
	{
		return sqrt(x * x + y * y);
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

	bool operator==(Vector3 number) const {
		return (x == number.x && y == number.y && z == number.z);
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
	std::string Print()
	{

		std::stringstream ss;
		ss << "Pitch : " << Pitch << " Yaw : " << Yaw << " Roll : " << Roll;
		return ss.str();

	}
};