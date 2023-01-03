#pragma once

#include "includes.h"

#include "unreal/color.h"
#include "keybind.h"

class CConfigItem
{
public:
	const char* Type;
	const char* Name;
	void* Value;
};

class CConfig : public Singleton<CConfig>
{

private:
	
	std::vector <CConfigItem> Items{};

	void AddConfigItem(void* Pointer, const char* Name, const char* Type);

public:

	void RefreshConfigs();
	void LoadConfig(int ConfigID);
	void SaveConfig();

	bool InitializeConfig();

	void* GetItem(const char* Name);

};

namespace Config
{

	inline bool EnableNameEsp = false;
	inline bool EnableBoxEsp = false;
	inline int BoxEspStyle = 0;
	inline bool Enable3DBoxes = false;
	inline bool EnableConsoleEsp = false;
	inline bool EnableSkeletonEsp = false;
	inline bool EnableCurrentWeaponEsp = false;
	inline bool EnableDistanceEsp = true;
	inline bool EnableSnaplines = true;
	inline int SnaplineStart = 0;

	inline bool EnableBotCheck = true;
	inline bool EnableTeamCheck = true;
	inline bool VisibleOnlyEsp = false;

	inline float MaxEspDistance = 250.f;

	inline Color NameEspColorVisible{ 1.f, 0.f, 0.f, 1.f };
	inline Color NameEspColorInVisible{ 0.f, 1.f, 0.f, 1.f };

	inline Color BoxEspColorVisible{ 1.f, 0.f, 0.f, 1.f };
	inline Color BoxEspColorInVisible{ 0.f, 1.f, 0.f, 1.f };

	inline Color SkeletonEspColorVisible{ 1.f, 0.f, 0.f, 1.f };
	inline Color SkeletonEspColorInVisible{ 0.f, 1.f, 0.f, 1.f };

	inline Color CurrentWeaponEspColorVisible{ 1.f, 0.f, 0.f, 1.f };
	inline Color CurrentWeaponEspColorInVisible{ 0.f, 1.f, 0.f, 1.f };

	inline Color SnaplinesColorVisible{ 1.f, 0.f, 0.f, 1.f };
	inline Color SnaplinesColorInVisible{ 0.f, 1.f, 0.f, 1.f };

	inline bool EnableSpinbot = false;
	inline CKeybind SpinbotKey{ "enable spinbot" };
	inline bool EnableNoRecoil = false;
	inline bool EnableNoSpread = false;

	inline bool EnableAimbot = false;
	inline bool EnableAimbotSmoothing = true;
	inline float AimbotSmoothing = 1.f;
	inline float AimbotFOV = 100.f;
	inline int AimbotHitbox = 0;
	inline bool VisibleOnlyAimbot = true;
	inline float MaxAimbotDistance = 100.f;

	inline Color FOVColor{ 0.f, 0.f, 1.f, 1.f };
	inline bool ShowFOV = true;

	inline CKeybind AimbotKey{ "enable aimbot" };

	inline bool EnableWatermark = true;
	inline bool EnableCrosshair = true;
	inline int CrosshairStyle = 0;
	inline bool KeybindList = true;
	inline Color CrosshairColor{ 1.f, 1.f, 1.f, 1.f };
	inline bool EnableInstantWeaponDeploy = false;

}