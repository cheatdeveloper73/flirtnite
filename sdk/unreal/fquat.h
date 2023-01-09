#pragma once

#include "../includes.h"
#include "vector3.h"

FORCEINLINE float ClampAxis(float Angle)
{
	// returns Angle in the range (-360,360)
	Angle = fmod(Angle, 360.0);

	if (Angle < 0.0)
	{
		// shift to [0,360) range
		Angle += 360.0;
	}

	return Angle;
}

FORCEINLINE float NormalizeAxis(float Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > 180.0)
	{
		// shift to (-180,180]
		Angle -= 360.0;
	}

	return Angle;
}

struct FQuat
{
	double X, Y, Z, W;
	FRotator Rotator()
	{
		const float SingularityTest = Z * X - W * Y;
		const float YawY = 2.f * (W * Z + X * Y);
		const float YawX = (1.f - 2.f * (Y*Y + Z*Z));

		// reference 
		// http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/

		// this value was found from experience, the above websites recommend different values
		// but that isn't the case for us, so I went through different testing, and finally found the case 
		// where both of world lives happily. 
		const float SINGULARITY_THRESHOLD = 0.4999995f;
		const float RAD_TO_DEG = (180.f / 3.1415926535897932384626433832795);
		float Pitch, Yaw, Roll;

		if (SingularityTest < -SINGULARITY_THRESHOLD)
		{
			Pitch = -90.f;
			Yaw = (atan2(YawY, YawX) * RAD_TO_DEG);
			Roll = NormalizeAxis(-Yaw - (2.f * atan2(X, W) * RAD_TO_DEG));
		}
		else if (SingularityTest > SINGULARITY_THRESHOLD)
		{
			Pitch = 90.f;
			Yaw = (atan2(YawY, YawX) * RAD_TO_DEG);
			Roll = NormalizeAxis(Yaw - (2.f * atan2(X, W) * RAD_TO_DEG));
		}
		else
		{
			Pitch = (asin(2.f * SingularityTest) * RAD_TO_DEG);
			Yaw = (atan2(YawY, YawX) * RAD_TO_DEG);
			Roll = (atan2(-2.f * (W * X + Y * Z), (1.f - 2.f * (X*X + Y*Y))) * RAD_TO_DEG);
		}

		FRotator RotatorFromQuat = FRotator(Pitch, Yaw, Roll);

		return RotatorFromQuat;
	}
};