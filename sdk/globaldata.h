#pragma once

#include "includes.h"
#include "memory.h"
#include "game.h"

namespace offsets
{

	constexpr uintptr_t GNames = 0xea10b00;
	constexpr uintptr_t GObjects = 0xe820b00;
	constexpr uintptr_t GWorld = 0xe9ea3f8;

}

namespace hashes
{

    const inline auto Pawn_Athena = FNV1A::HashConst("PlayerPawn_Athena_C");
    const inline auto Pawn_Phobe = FNV1A::HashConst("BP_PlayerPawn_Athena_Phoebe_C");
    const inline auto BP_MangPlayerPawn = FNV1A::HashConst("BP_MangPlayerPawn");

    const inline auto FortPickupAthena = FNV1A::HashConst("FortPickupAthena");
    const inline auto Tiered_Chest = FNV1A::HashConst("Tiered_Chest");
    const inline auto Tiered_Ammo = FNV1A::HashConst("Tiered_Ammo");
    const inline auto Vehicle = FNV1A::HashConst("Vehicle");

    const inline auto FortWorldSettings = FNV1A::HashConst("FortWorldSettings");

}

class CGlobalData : public Singleton<CGlobalData>
{

public:

	UnrealEngine::Pawn* LocalPawn = nullptr;
	UnrealEngine::UWorld* UWorld = nullptr;
    UnrealEngine::GameInstance* GameInstance = nullptr;
    UnrealEngine::PlayerController* LocalController = nullptr;
    UnrealEngine::PersistentLevel* Level = nullptr;
    uintptr_t LocalPlayer = NULL;
    Camera PlayerCamera{};

    std::vector <UnrealEngine::CCachedPlayer> Players{};
    std::vector <UnrealEngine::CCachedEntity> Entities{};
    std::mutex PlayerSync{};

    static std::string GetNameFromIndex(int key)
    {
        uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
        uint16_t NameOffset = (uint16_t)key;
        uint64_t NamePoolChunk = Memory.Read<uint64_t>(Memory.BaseAddress + offsets::GNames + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
        uint16_t nameEntry = Memory.Read<uint16_t>(NamePoolChunk);
        int nameLength = nameEntry >> 6;
        char buff[1024] = {};

        char* v2; // rdi
        int v8; // ecx
        int16_t v9; // ax
        int16_t result = 38i64;
        int v6 = 0;
        int v7 = 38;
        if (nameLength)
        {
            Memory._HyperV->ReadMem(PVOID(NamePoolChunk + 4), (PVOID)(&buff), 2 * nameLength);
            v2 = buff;
            do
            {
                v8 = v6++ | v7;
                v9 = v8;
                v7 = 2 * v8;
                result = ~v9;
                *v2 ^= result;
                ++v2;
            } while (v6 < nameLength);
            buff[nameLength] = '\0';
            return std::string(buff);
        }
        return std::string("");
    }

    static std::string GetNameFromFName(int key)
    {
        uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
        uint16_t NameOffset = (uint16_t)key;

        uint64_t NamePoolChunk = Memory.Read<uint64_t>(Memory.BaseAddress + offsets::GNames + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
        if (Memory.Read<uint16_t>(NamePoolChunk) < 64)
        {
            auto a1 = Memory.Read<DWORD>(NamePoolChunk + 4);
            return GetNameFromIndex(a1);
        }
        else
        {
            return GetNameFromIndex(key);
        }
    }

	void CacheEntities();
	void CacheLoot();

};