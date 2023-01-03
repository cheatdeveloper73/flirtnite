#include "globaldata.h"

#include "config.h"

std::string decrypt_player_name(uintptr_t pState)
{
	int pNameLength; // rsi
	WORD* pNameBufferPointer;
	int i; // ecx
	char v25; // al
	int v26; // er8
	int v29; // eax

	uintptr_t pNameStructure = Memory.Read<uintptr_t>(pState + 0xB08); // you won't get help for that offset though
	pNameLength = Memory.Read<int>(pNameStructure + 0x10);
	if (pNameLength <= 0) return "";

	wchar_t* pNameBuffer = new wchar_t[pNameLength];
	uintptr_t pNameEncryptedBuffer = Memory.Read<uintptr_t>(pNameStructure + 0x8);
	Memory._HyperV->ReadMem((PVOID)pNameEncryptedBuffer, pNameBuffer, pNameLength * sizeof(wchar_t));

	v25 = pNameLength - 1;
	v26 = 0;
	pNameBufferPointer = (WORD*)pNameBuffer;

	for (i = (v25) & 3; ; *pNameBufferPointer++ += i & 7)
	{
		v29 = pNameLength - 1;
		if (!(DWORD)pNameLength)
			v29 = 0;

		if (v26 >= v29)
			break;

		i += 3;
		++v26;
	}

	std::wstring temp_wstring(pNameBuffer);
	delete[] pNameBuffer;
	return std::string(temp_wstring.begin(), temp_wstring.end());
}

void CGlobalData::CacheEntities()
{

	while (true)
	{

		static uint32_t LastListSize = 0;

		UWorld = Memory.Read<UnrealEngine::UWorld*>(Memory.BaseAddress + offsets::GWorld);

		if (!UWorld)
		{
			WRAP_IF_DEBUG(
				std::cout << "Failed to get UWorld.\n";
			)
			continue;
		}

		GameInstance = UWorld->Instance();

		if (!GameInstance)
		{
			WRAP_IF_DEBUG(
				std::cout << "Failed to get the GameInstance.\n";
			)
			continue;
		}

		LocalController = GameInstance->LocalPlayerController();

		if (!LocalController)
		{
			WRAP_IF_DEBUG(
				std::cout << "Failed to get the LocalPlayerController.\n";
			)
			continue;
		}

		LocalPawn = LocalController->Pawn();

		if (!LocalPawn)
		{
			WRAP_IF_DEBUG(
				std::cout << "Failed to get the local Pawn.\n";
			)
			continue;
		}

		Level = UWorld->Level();
		
		if (!Level)
		{
			WRAP_IF_DEBUG(
				std::cout << "Failed to get the Level.\n";
			)
			continue;
		}

		LocalPlayer = GameInstance->LocalPlayer();

		uint32_t ActorCount = Level->ActorCount();

		if (ActorCount == LastListSize)
			continue;

		uintptr_t ActorArray = Level->ActorArray();

		if (!ActorArray || ActorCount < 1)
			continue;

		WRAP_IF_DEBUG(
			std::cout << "Current object count: " << ActorCount << "\n";
		)

		std::vector <UnrealEngine::CCachedPlayer> TmpCache{};
		std::vector <UnrealEngine::CCachedEntity> TmpEntityCache{};

		std::vector <UnrealEngine::Pawn*> PawnList = Memory._HyperV->ReadVector<UnrealEngine::Pawn*>(ActorArray, ActorCount);

		for (const auto& Pawn : PawnList)
		{

			if (!Pawn)
			{
				WRAP_IF_DEBUG(
					std::cout << "Skipping NULL pawn (buffer list!)\n";
				);
				continue;
			}

#ifndef _DEBUG
			if (Pawn == LocalPawn)
			{
				WRAP_IF_DEBUG(
					std::cout << "Skipping local pawn (buffer list)!\n";
				)
					continue;
			}
#endif // !_DEBUG

			const auto PawnName = FNV1A::Hash(GetNameFromFName(Pawn->ID()).c_str());

			if (PawnName == hashes::Pawn_Athena || PawnName == hashes::Pawn_Phobe || PawnName == hashes::BP_MangPlayerPawn)
			{

				auto State = Pawn->PlayerState();
				auto Weapon = Pawn->Weapon();

				if (!State)
					continue;

				if (!Config::EnableTeamCheck && State->TeamID() == LocalPawn->PlayerState()->TeamID())
					continue;

				UnrealEngine::CCachedPlayer CachePlayer{};
				CachePlayer.Pawn = Pawn;
				CachePlayer.Mesh = Pawn->Mesh();
				CachePlayer.PlayerState = State;
				CachePlayer.Weapon = Weapon;
				CachePlayer.Name = decrypt_player_name((uintptr_t)State);
				CachePlayer.IsBot = CachePlayer.Name == ""; // bots don't have a name when decrypted
				if (Config::EnableCurrentWeaponEsp)
				{
					uintptr_t WeaponNamePTR = Weapon->Definition()->Name();
					if (!Weapon || !WeaponNamePTR)
						CachePlayer.WeaponName = "None";
					else
					{
						uint32_t WeaponNameLength = Memory.Read<uint32_t>(WeaponNamePTR + 0x38);
						wchar_t* WeaponName = new wchar_t[WeaponNameLength + 1];
						Memory._HyperV->ReadMem(Memory.Read<PVOID>(WeaponNamePTR + 0x30), WeaponName, WeaponNameLength * sizeof(wchar_t));
						std::wstring Wide(WeaponName);
						delete[] WeaponName;
						CachePlayer.WeaponName = std::string(Wide.begin(), Wide.end());
					}
				}
				if (Config::EnableConsoleEsp)
				{
					uintptr_t Platform_FString = Memory.Read<uintptr_t>((uintptr_t)State + 0x490);
					wchar_t Platform[64];
					Memory._HyperV->ReadMem((PVOID)Platform_FString, Platform, sizeof(Platform));
					std::wstring wide(Platform);
					std::string str_platform(wide.begin(), wide.end());
					CachePlayer.Platform = str_platform;
				}

				TmpCache.emplace_back(CachePlayer);

				WRAP_IF_DEBUG(
					std::cout << "Added player to the list!\n";
				)

					continue;

			}
		}



		PlayerSync.lock();
		Players.clear();
		Entities.clear();
		Players = TmpCache;
		Entities = TmpEntityCache;
		PlayerSync.unlock();

		LastListSize = ActorCount;

		Sleep(2000);

	}

}