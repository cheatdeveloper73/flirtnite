#pragma once

#include "memory.h"
#include "includes.h"
#include "../sdk/unreal/vector3.h"
#include "../sdk/unreal/camera.h"
#include "../sdk/unreal/ftransform.h"
#include "../sdk/unreal/tarray.h"
#include "../sdk/unreal/string.h"
#include "../sdk/unreal/color.h"

#define cast_this reinterpret_cast<uint64_t>(this)
#define get_member(type, name, offset) type name() { return Memory.Read<type>(cast_this + offset); } 
#define set_member(type, name, offset) void name( type val ) { Memory.Write<type>(cast_this + offset, val); }

namespace UnrealEngine
{

	struct FMinimalViewInfo
	{
		Vector3 Location{0.f, 0.f, 0.f};
		Vector3 Rotation{0.f, 0.f, 0.f};
		float FOV = -300.f;
	};

	struct FBoxSphereBounds
	{
		struct Vector3                                     Origin;                                                   // 0x0000(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		struct Vector3                                     BoxExtent;                                                // 0x0018(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		double                                             SphereRadius;                                             // 0x0030(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	};

	struct OtherViewInf
	{
		int trash;
		Vector3 Location;
		Vector3 Rotation;
		float FOV;
	};

	struct ViewTarget
	{
		void* Target;
		OtherViewInf POV;
	};

	struct FCameraCacheEntry
	{
		float TimeStamp;
		FMinimalViewInfo POV;
	};

	struct PlayerCameraManager
	{
		void* PCOwner;
		void* TransformComponent;
		void* CameraStyle;
		float LockedFOV;
		float DefaultOrthoWidth;
		float LockedOrthoWidth;
		float DefaultAspectRatio;
		Color FadeColor;
		float FadeAmount;
		Vector3 ColorScale;
		Vector3 DesiredColorScale;
		Vector3 OriginalColorScale;
		float ColorScaleInterpDuration;
		float ColorScaleInterpStartTime;
		FCameraCacheEntry CameraCache;
		FCameraCacheEntry LastFrameCameraCache;
		ViewTarget ViewTarget;
	};

	class Mesh
	{
	public:
		get_member(float, LastSubmitTime, 0x330)
		get_member(float, LastRenderTime, 0x338)
		get_member(FTransform, ComponentToWorld, 0x240)
		get_member(Vector3, Unknown, 0x140)
		bool IsVisible()
		{
			return LastRenderTime() + 0.015f >= LastSubmitTime();
		}
		FTransform GetBone(int index)
		{

			auto BoneArray = Memory.Read<uintptr_t>(cast_this + 0x5C0);

			if (!BoneArray)
				BoneArray = Memory.Read<uintptr_t>(cast_this + (0x5C0 + 0x10));

			return Memory.Read<FTransform>(BoneArray + (index * 0x60));
		}
		Vector3 GetBonePos(int index)
		{
			auto Bone = GetBone(index);
			auto ComponentToWorld = Mesh::ComponentToWorld();
			D3DMATRIX Matrix = MatrixMultiplication(Bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
			auto Pos = Vector3{ Matrix._41, Matrix._42, Matrix._43 };
			return Pos;
		}
	};
	
	class PlayerState
	{
	public:
		get_member(uint32_t, TeamID, 0x6A1)
		get_member(float, Health, 0xe54)
		get_member(float, Shield, 0xe5c)
	};

	struct FText {
		FString* text[0x8];
		char pad_dontcare[0x30];
	};

	class WeaponDefinition
	{
	public:
		get_member(uint32_t, ID, 0x18)
		get_member(uintptr_t, Name, 0x90)
	};

	class Weapon
	{
	public:
		get_member(int, die, 1000);
		get_member(WeaponDefinition*, Definition, 0x3f0)
	};

	class SceneComponenet
	{
	public:
		get_member(FRotator, RelativeRotation, 0x140)
		set_member(FRotator, RelativeRotation, 0x140)
	};

	class Pawn
	{
	public:
		Vector3 RelativeLocation()
		{
			return Memory.Read<Vector3>(Memory.Read<uintptr_t>(cast_this + 0x190) + 0x128);
		}
		get_member(SceneComponenet*, RootComponent, 0x190);
		get_member(uint32_t, ID, 0x18);
		get_member(UnrealEngine::PlayerState*, PlayerState, 0x2A8);
		get_member(UnrealEngine::Mesh*, Mesh, 0x310);
		set_member(bool, RimlightEnabled, 0x5ec0)
		set_member(double, RimlightMultiplier, 0x5eb8)
		get_member(UnrealEngine::Weapon*, Weapon, 0x8d8)
		get_member(uintptr_t, ClassPrivate, 0x10)
	};

	class PlayerController
	{
	public:
		UnrealEngine::Pawn* Pawn()
		{

			UnrealEngine::Pawn* pwn = Memory.Read<UnrealEngine::Pawn*>(cast_this + 0x330);

			if (!pwn) // lobby/in bus pawn?
				pwn = Memory.Read<UnrealEngine::Pawn*>(cast_this + 0x328);

			return pwn;

		}
		get_member(float, Time, 0x64)
		set_member(float, Time, 0x64)
		get_member(FRotator, ControlRotation, 0x300)
		set_member(FRotator, ControlRotation, 0x300)
		get_member(uintptr_t, CameraManager, 0x340)

	};

	class GameInstance
	{
	public:
		uintptr_t LocalPlayer()
		{
			uintptr_t LocalPlayers = Memory.Read<uintptr_t>(cast_this + 0x38);
			uintptr_t LocalPlayer = Memory.Read<uintptr_t>(LocalPlayers);
			return LocalPlayer;
		}
		get_member(uintptr_t, LocalPlayerArray, 0x38)
		PlayerController* LocalPlayerController()
		{
			return Memory.Read<PlayerController*>(LocalPlayer() + 0x30);
		}
	};

	class PersistentLevel
	{
	public:
		get_member(uintptr_t, ActorArray, 0x98);
		get_member(uint32_t, ActorCount, 0xA0);
	};

	class UWorld
	{
	public:
		get_member(GameInstance*, Instance, 0x1B8);
		get_member(PersistentLevel*, Level, 0x30);
		get_member(uintptr_t, CameraPointer, 0x110);
		get_member(uintptr_t, LevelList, 0x160);
		get_member(uint32_t, LevelCount, 0x160+0x8)
	};

	// 0xe9ea3f8

	inline Camera GetCamera(UnrealEngine::PlayerController* Controller, UnrealEngine::Pawn* LocalPawn, UnrealEngine::UWorld* UWorld)
	{

		//Camera Camera;
		//
		//uintptr_t RotationChain = Memory.Read<uintptr_t>(Memory.Read<uintptr_t>((uintptr_t)LocalPlayer + 0xD0) + 0x8);

		//if (!RotationChain)
		//	return Camera;

		//Camera.Rotation.x = asin(Memory.Read<double>(RotationChain + 0x9C0)) * (180.0 / M_PI);
		//Camera.Rotation.y = Memory.Read<double>(Memory.Read<uintptr_t>((uintptr_t)LocalPawn + 0x190) + 0x148);
		//Camera.Rotation.z = 0.f;

		//Camera.Location = Memory.Read<Vector3>(RotationChain + 0x4C0);

		//Camera.FOV = Memory.Read<float>(Memory.Read<uintptr_t>(LocalPlayer + 0x30) + 0x38C) * 90.f;

		//Camera.Valid = true;

		//return Camera;

		//Camera Camera;

		//auto manager = Controller->CameraManager();
		//auto viewtarget = Memory.Read<UnrealEngine::ViewTarget>(manager + 0x12c0);

		//Camera.FOV = viewtarget.POV.FOV;
		//Camera.Valid = true;
		//Camera.Location = viewtarget.POV.Location;
		//Camera.Rotation = viewtarget.POV.Rotation;


	}

	inline bool WorldToScreen(Vector3 Location, Vector2& Out, Camera Cam, int Width, int Height)
	{

		D3DMATRIX tempMatrix = Matrix(Cam.Rotation);

		Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		Vector3 vDelta = Location - Cam.Location;
		Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

		if (vTransformed.z < 1.f)
			return false;

		double ScreenX = (Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(Cam.FOV * (float)M_PI / 360.f))) / vTransformed.z;
		double ScreenY = (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(Cam.FOV * (float)M_PI / 360.f))) / vTransformed.z;

		Out = Vector2(ScreenX, ScreenY);

		return true;

	}

	class CCachedPlayer
	{
	public:
		UnrealEngine::Pawn* Pawn;
		UnrealEngine::Mesh* Mesh;
		UnrealEngine::PlayerState* PlayerState;
		std::string WeaponName;
		UnrealEngine::Weapon* Weapon;
		std::string Name = "";
		std::string Platform = "";
		bool IsBot = false;
	};

	class CCachedEntity
	{
	public:
		const char* EntityName;
		Vector3 EntityPosition;
		bool IsSearched;
		int Type; // 0 - chest, 1 - vehicle, 2 - pickup, 3 - other
		int32_t Count;
	};

}