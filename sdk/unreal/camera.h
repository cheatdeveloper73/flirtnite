#pragma once

#include "../includes.h"

#include "vector3.h"

struct Camera // FMinimalViewInfo
{

	float FOV;
	Vector3 Rotation;
	Vector3 Location;
	bool Valid = false;

};