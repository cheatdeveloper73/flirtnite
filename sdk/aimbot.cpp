#pragma once

#include "aimbot.h"

CAimTarget CAimbot::FindTarget(int width, int height)
{

	std::vector <CAimTarget> TmpTargets{};

	for (const auto& Player : CGlobalData::Get().Players)
	{

		if (Config::VisibleOnlyAimbot && !Player.Mesh->IsVisible())
			continue;

		switch (Config::AimbotHitbox)
		{

		case 0: // head
		{

			Vector3 HeadPos = Player.Mesh->GetBonePos(68);
			Vector2 HeadScreen{};
			if (!UnrealEngine::WorldToScreen(HeadPos, HeadScreen, CGlobalData::Get().PlayerCamera, width, height))
				continue; // player isn't visible
			CAimTarget Target;
			float FOV = HeadScreen.Distance(Vector2{ width / 2.f, height / 2.f });

			Target.AimPOS = HeadScreen;
			Target.Validated = true;
			Target.FOV = FOV;
			Target.Pawn = Player.Pawn;
			Target.VecAimPos = HeadPos;
			Target.Pawn = Player.Pawn;

			TmpTargets.push_back(Target);

			break;
		}
		case 1: // chest
		{

			Vector3 HeadPos = Player.Mesh->GetBonePos(5);
			Vector2 HeadScreen{};
			if (!UnrealEngine::WorldToScreen(HeadPos, HeadScreen, CGlobalData::Get().PlayerCamera, width, height))
				continue; // player isn't visible
			CAimTarget Target;
			float FOV = HeadScreen.Distance(Vector2{ width / 2.f, height / 2.f });

			Target.AimPOS = HeadScreen;
			Target.Validated = true;
			Target.FOV = FOV;
			Target.Pawn = Player.Pawn;
			Target.VecAimPos = HeadPos;

			TmpTargets.push_back(Target);

			break;
		}
		case 2: // nearest
		{

			Vector2 FinalPoint;
			float FinalFOV;
			Vector3 FinalVecPos;

			Vector3 HeadPos = Player.Mesh->GetBonePos(68);
			Vector3 BodyPos = Player.Mesh->GetBonePos(5);

			Vector2 HeadScreen;
			Vector2 BodyScreen;

			if (!UnrealEngine::WorldToScreen(HeadPos, HeadScreen, CGlobalData::Get().PlayerCamera, width, height))
				continue;
			if (!UnrealEngine::WorldToScreen(HeadPos, BodyScreen, CGlobalData::Get().PlayerCamera, width, height))
				continue;

			float HeadFOV = CalculateFOV(HeadScreen);
			float BodyFOV = CalculateFOV(BodyScreen);

			if (BodyFOV > Config::AimbotFOV)
				continue; // they're out of fov
			if (HeadFOV > Config::AimbotFOV)
				continue;

			if (HeadFOV < BodyFOV)
			{
				FinalPoint = HeadScreen;
				FinalFOV = HeadFOV;
				FinalVecPos = HeadPos;
			}
			else
			{
				FinalPoint = BodyScreen;
				FinalFOV = BodyFOV;
				FinalVecPos = BodyPos;
			}

			CAimTarget Target;

			Target.AimPOS = FinalPoint;
			Target.Validated = true;
			Target.FOV = FinalFOV;
			Target.Pawn = Player.Pawn;

			TmpTargets.push_back(Target);

			break;
		}

		}

	}

	auto SortByFOV = [](CAimTarget t1, CAimTarget t2) {

		return t1.FOV < t2.FOV;

	};

	if (TmpTargets.size() > 0)
		std::sort(TmpTargets.begin(), TmpTargets.end(), SortByFOV);
	else
		return CAimTarget{};

	CAimTarget BestTarget = TmpTargets[0];
	TmpTargets.clear();

	if (!BestTarget.Validated)
		return CAimTarget{};

	return BestTarget;

}

FRotator MyLookRotation(Vector3 From, Vector3 lookAt, Vector3 upDirection)
{
	Vector3 forward = lookAt - From;
	Vector3 up = upDirection;


	forward = forward.GetSafeNormal();
	up = up - (forward * up.Dot(forward));
	up = up.GetSafeNormal();

	///////////////////////

	Vector3 vector = forward.GetSafeNormal();
	Vector3 vector2 = up.Cross(vector);
	Vector3 vector3 = vector.Cross(vector2);
	float m00 = vector.x;
	float m01 = vector.y;
	float m02 = vector.z;
	float m10 = vector2.x;
	float m11 = vector2.y;
	float m12 = vector2.z;
	float m20 = vector3.x;
	float m21 = vector3.y;
	float m22 = vector3.z;

	float num8 = (m00 + m11) + m22;
	FQuat quaternion = FQuat();

	if (num8 > 0.0f)
	{
		float num = (float)sqrt(num8 + 1.0f);
		quaternion.W = num * 0.5f;
		num = 0.5f / num;
		quaternion.X = (m12 - m21) * num;
		quaternion.Y = (m20 - m02) * num;
		quaternion.Z = (m01 - m10) * num;
		return quaternion.Rotator();
	}

	if ((m00 >= m11) && (m00 >= m22))
	{
		float num7 = (float)sqrt(((1.0f + m00) - m11) - m22);
		float num4 = 0.5f / num7;
		quaternion.X = 0.5f * num7;
		quaternion.Y = (m01 + m10) * num4;
		quaternion.Z = (m02 + m20) * num4;
		quaternion.W = (m12 - m21) * num4;
		return quaternion.Rotator();
	}

	if (m11 > m22)
	{
		float num6 = (float)sqrt(((1.0f + m11) - m00) - m22);
		float num3 = 0.5f / num6;
		quaternion.X = (m10 + m01) * num3;
		quaternion.Y = 0.5f * num6;
		quaternion.Z = (m21 + m12) * num3;
		quaternion.W = (m20 - m02) * num3;
		return quaternion.Rotator();
	}

	float num5 = (float)sqrt(((1.0f + m22) - m00) - m11);
	float num2 = 0.5f / num5;
	quaternion.X = (m20 + m02) * num2;
	quaternion.Y = (m21 + m12) * num2;
	quaternion.Z = 0.5f * num5;
	quaternion.W = (m01 - m10) * num2;


	return quaternion.Rotator();
}

double deg2rad(double deg) {
	return deg * M_PI / 180.0;
}

Vector3 CalcAngle(Vector3 src, Vector3 dst)
{
#define M_RADPI 57.295779513082f
	Vector3 angles;

	Vector3 delta = src - dst;
	float hyp = delta.Length2D();

	angles.y = std::atanf(delta.y / delta.x) * M_RADPI;
	angles.x = std::atanf(-delta.z / hyp) * -M_RADPI;
	angles.z = 0.0f;

	return angles;

}

double SignedAngleTo(Vector3 source, Vector3 dest, Vector3 planeNormal)
{
	return 1.f;
}

void CAimbot::Run(int width, int height)
{

	if (!Config::AimbotKey.enabled || !Config::EnableAimbot)
		return; // avoid aimbotting if we don't wanna duh

	CAimTarget Target = FindTarget(width, height);

	if (!Target.Validated)
		return; // no targets

	if (Target.FOV > Config::AimbotFOV)
		return;

	float Distance = CGlobalData::Get().PlayerCamera.Location.Distance(Target.VecAimPos) / 100.f;

	if (Distance > Config::MaxAimbotDistance)
		return;

	if (Config::AimbotKey.enabled)
		MoveMouse(Target.AimPOS.x, Target.AimPOS.y, width, height);

}

void CAimbot::AddTarget(UnrealEngine::Pawn* Pawn, float Distance, bool Visible)
{

	Targets.push_back({ Visible, Distance, Pawn });

}

void CAimbot::MoveMouse(int x, int y, int width, int height)
{

	float ScreenCenterX = (width / 2);
	float ScreenCenterY = (height / 2);

	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			if (Config::EnableAimbotSmoothing)
				TargetX /= Config::AimbotSmoothing;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			if (Config::EnableAimbotSmoothing)
				TargetX /= Config::AimbotSmoothing;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			if (Config::EnableAimbotSmoothing)
				TargetY /= Config::AimbotSmoothing;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			if (Config::EnableAimbotSmoothing)
				TargetY /= Config::AimbotSmoothing;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);

}

float CAimbot::CalculateFOV(Vector2 Pos)
{

	ImVec2 display_size = ImGui::GetIO().DisplaySize;
	Vector2 screen_center(display_size.x / 2, display_size.y / 2);

	return Pos.Distance(screen_center);

}