#pragma once

#include "includes.h"

#include "memory.h"
#include "config.h"
#include "game.h"
#include "globaldata.h"

class CAimTarget
{
public:
	bool Visible;
	float Distance;
	UnrealEngine::Pawn* Pawn;
	float FOV;
	bool Validated = false;
	Vector2 AimPOS;
	int BoneID;
	Vector3 VecAimPos;
};

class CAimbot : public Singleton<CAimbot>
{

private:
	void MoveMouse(int x, int y, int width, int height);
	float CalculateFOV(Vector2 Pos);
	CAimTarget FindTarget(int width, int height);

	std::vector <CAimTarget> Targets{};

public:
	void Run(int width, int height);
	void AddTarget(UnrealEngine::Pawn* Pawn, float Distance, bool Visible);

};