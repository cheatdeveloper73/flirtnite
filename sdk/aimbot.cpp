#pragma once

#include "aimbot.h"

CAimTarget CAimbot::FindTarget(int width, int height)
{

	std::vector <CAimTarget> TmpTargets{};

	CGlobalData::Get().PlayerSync.lock();

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
			Target.BoneID = 68;
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

			TmpTargets.push_back(Target);

			break;
		}
		case 2: // nearest
		{

			Vector2 FinalPoint;
			float FinalFOV;

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
			}
			else
			{
				FinalPoint = BodyScreen;
				FinalFOV = BodyFOV;
			}

			CAimTarget Target;

			Target.AimPOS = FinalPoint;
			Target.Validated = true;
			Target.FOV = FinalFOV;

			TmpTargets.push_back(Target);

			break;
		}

		}

	}

	CGlobalData::Get().PlayerSync.unlock();

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

void CAimbot::Run(int width, int height)
{

	if (!Config::AimbotKey.enabled)
		return; // avoid aimbotting if we don't wanna duh

	CAimTarget Target = FindTarget(width, height);

	if (!Target.Validated)
		return; // no targets

	if (Target.FOV > Config::AimbotFOV)
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
			TargetX /= Config::AimbotSmoothing;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= Config::AimbotSmoothing;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= Config::AimbotSmoothing;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
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