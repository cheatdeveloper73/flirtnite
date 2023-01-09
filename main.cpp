#include "sdk/includes.h"

#include "ImGuiRender/ImGuiRender.h"
#include "Overlay/Overlay.h"

#include "sdk/memory.h"
#include "menu/menu.h"
#include "sdk/globaldata.h"
#include "sdk/config.h"
#include "sdk/aimbot.h"

#include "sdk/decryption.h"

COverlay* c_overlay = new COverlay();
bool clickable = false;

void change_click(bool canclick)
{
	long style = ::GetWindowLong(c_overlay->GetLocalHwnd(), GWL_EXSTYLE);
	if (canclick)
	{
		style &= ~WS_EX_LAYERED;
		SetWindowLong(c_overlay->GetLocalHwnd(), GWL_EXSTYLE, style);
		SetFocus(c_overlay->GetLocalHwnd());
		clickable = true;
	}
	else
	{
		style |= WS_EX_LAYERED;
		SetWindowLong(c_overlay->GetLocalHwnd(), GWL_EXSTYLE, style);
		SetFocus(c_overlay->GetLocalHwnd());
		clickable = false;
	}
}

__forceinline bool W2S(Vector3 pos, Vector2& out)
{
	return UnrealEngine::WorldToScreen(pos, out, CGlobalData::Get().PlayerCamera, c_overlay->m_pWidth, c_overlay->m_pHeight);
}

ImVec2 x(Vector3 v)
{
	Vector2 tmp;
	W2S(v, tmp);
	return ImVec2(tmp.x, tmp.y);
}

__forceinline ImVec2 ProjectWorldToScreen(Vector3 p)
{
	Vector2 tmp;
	W2S(p, tmp);
	return ImVec2(tmp.x, tmp.y);
}

void EntityCacheThread()
{

	CGlobalData::Get().CacheEntities();

}

inline void DrawBox(ImVec2 pos, ImVec2 size, ImColor color)
{
	const ImRect rect_bb(pos, pos + size);

	ImGui::GetBackgroundDrawList()->AddRect(rect_bb.Min - ImVec2(1, 1), rect_bb.Max + ImVec2(1, 1), ImColor(0.f, 0.f, 0.f, color.Value.w));
	ImGui::GetBackgroundDrawList()->AddRect(rect_bb.Min + ImVec2(1, 1), rect_bb.Max - ImVec2(1, 1), ImColor(0.f, 0.f, 0.f, color.Value.w));
	ImGui::GetBackgroundDrawList()->AddRect(rect_bb.Min, rect_bb.Max, color);
}

inline void DrawHealth(int health, ImVec2 pos, ImVec2 size, float alpha, bool dormant)
{

	int max_health = 100;
	float health_ratio = health / (float)max_health;

	int offset = size.x - 2;
	offset -= (offset * health_ratio);

	ImColor color = dormant ? ImColor(52, 204, 235, int(200 * alpha)) : ImColor(int(244 - (116 * health_ratio)), int(100 + (144 * health_ratio)), 66, int(220 * alpha));

	ImRenderer->DrawFilledRect(ImVec2(pos.x - 5, pos.y), ImVec2(4, size.x), ImColor(0, 0, 0, int(130 * alpha)));
	ImRenderer->DrawFilledRect(ImVec2(pos.x - 4, pos.y + 1 + offset), ImVec2(2, size.x - 2 - offset), color);

}

class c_aim_target
{
public:
	UnrealEngine::Pawn* player;
	Vector3 aim_point;
	bool validated = false;
	float fov;
	float distance;
	bool visible;
};

float CalculateFOV(Vector3 position)
{

	Vector2 screen_position;

	if (!W2S(position, screen_position))
		return 1000.f;

	ImVec2 display_size = ImGui::GetIO().DisplaySize;
	Vector2 screen_center(display_size.x / 2, display_size.y / 2);

	return screen_position.Distance(screen_center);

}

int rotation = 0;

float BOG_TO_GRD(float BOG) {
	return (180 / M_PI) * BOG;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

Color rainbow_nazi(1.f, 0.f, 0.f, 1.f);

void ColorChange()
{
	if (1)
	{
		static float Color[3];
		static DWORD Tickcount = 0;
		static DWORD Tickcheck = 0;
		ImGui::ColorConvertRGBtoHSV(rainbow_nazi.r, rainbow_nazi.g, rainbow_nazi.b, Color[0], Color[1], Color[2]);
		if (GetTickCount() - Tickcount >= 1)
		{
			if (Tickcheck != Tickcount)
			{
				Color[0] += 0.001f * 5.f;
				Tickcheck = Tickcount;
			}
			Tickcount = GetTickCount();
		}
		if (Color[0] < 0.0f) Color[0] += 1.0f;
		ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], rainbow_nazi.r, rainbow_nazi.g, rainbow_nazi.b);
	}
}

int main_thread()
{

	if (!Memory.Initialize())
	{

		WRAP_IF_DEBUG(
			std::cout << "Failed to initialize memory.\n";
		)

		ERROR_MESSAGE("Failed to initialize. (Code: 0x1)");

		return 1;
	}

	ImRenderer = new GRenderer();
	if (!c_overlay->InitWindows(Memory.Window))
	{

		WRAP_IF_DEBUG(
			std::cout << "Failed to init windows.\n";
		)

		ERROR_MESSAGE("Failed to initialize. (Code: 0x2)");

		return 1;

	}

	SetWindowLongA(c_overlay->GetLocalHwnd(), GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

	RECT OldRc{};
	MSG msg{};

	WRAP_IF_DEBUG(
		std::cout << "PROCESS INFO DUMP -\n";
		std::cout << "Process Base: " << Memory.BaseAddress << "\n";
		std::cout << "Window HWND: " << Memory.Window << "\n";
		std::cout << "Process ID: " << Memory.GamePID << "\n";
		)

	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(EntityCacheThread), NULL, NULL, NULL);

	while (msg.message != WM_QUIT)
	{

		if (!FindWindowA("UnrealWindow", NULL))
		{
			Beep(1500, 3);
			exit(0);
		}

		if (GetAsyncKeyState(VK_INSERT) & 1)
			CMenu::Get().opened = !CMenu::Get().opened;

		if (PeekMessageA(&msg, c_overlay->GetLocalHwnd(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		HWND ForegaundWind = GetForegroundWindow();
		if (Memory.Window == ForegaundWind)
		{
			HWND GetWind = GetWindow(ForegaundWind, GW_HWNDPREV);
			SetWindowPos(c_overlay->GetLocalHwnd(), GetWind, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		RECT Rect{};
		POINT Cord{};
		GetClientRect(Memory.Window, &Rect);
		ClientToScreen(Memory.Window, &Cord);
		Rect.left = Cord.x;
		Rect.top = Cord.y;

		if (Rect.left != OldRc.left || Rect.right != OldRc.right || Rect.top != OldRc.top || Rect.bottom != OldRc.bottom)
		{
			OldRc = Rect;
			c_overlay->m_pWidth = (float)Rect.right;
			c_overlay->m_pHeight = (float)Rect.bottom;
			SetWindowPos(c_overlay->GetLocalHwnd(), 0, Cord.x, Cord.y, int(Rect.right + 2.f), int(Rect.bottom + 2.f), SWP_NOREDRAW);
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto shit = DecryptCamera();

		if (shit.FOV < 1.f) // we got an invalid cam :( (try pulling from viewtarget)
		{
			uintptr_t cam_manager = CGlobalData::Get().LocalController->CameraManager();
			UnrealEngine::ViewTarget viewtarget = Memory.Read<UnrealEngine::ViewTarget>(cam_manager + 0x12c0);
			CGlobalData::Get().PlayerCamera.FOV = viewtarget.POV.FOV;
			CGlobalData::Get().PlayerCamera.Location = viewtarget.POV.Location;
			CGlobalData::Get().PlayerCamera.Rotation = viewtarget.POV.Rotation;
			CGlobalData::Get().PlayerCamera.Valid = true;
		}
		else
		{
			CGlobalData::Get().PlayerCamera.FOV = shit.FOV;
			CGlobalData::Get().PlayerCamera.Location = shit.Location;
			CGlobalData::Get().PlayerCamera.Rotation = shit.Rotation;
			CGlobalData::Get().PlayerCamera.Valid = true;
		}

		Config::AimbotKey.update();
		Config::SpinbotKey.update();

		auto LocalPawn = (uintptr_t)CGlobalData::Get().LocalPawn;

		if (LocalPawn)
		{

			auto Weapon = CGlobalData::Get().LocalPawn->Weapon();

			if (Config::EnableNoSpread)
				Memory.Write<float>((uintptr_t)Weapon + 0x64, FLT_MAX);

			if (Config::EnableNoRecoil)
				CGlobalData::Get().LocalController->Time(-1.f);

			if (Config::EnableInstantWeaponDeploy)
				Memory.Write<bool>((uintptr_t)Weapon + 0x32b, true);

		}

		if (CMenu::Get().opened)
		{
			if (clickable != true)
				change_click(true);

			CMenu::Get().Render();

		}
		else
		{
			if (clickable != false)
				change_click(false);
		}	

		if (Config::EnableWatermark)
		{

			std::stringstream ss;

			ss << Config::Username << " ~ ";
			ss << "fps / " << static_cast<int>(ImGui::GetIO().Framerate);

			ImGui::SetNextWindowPos(ImVec2(5, 5));
			ImGui::SetNextWindowSize(ImVec2(ImGui::CalcTextSize(ss.str().c_str()).x + 15, 10));
			ImGui::Begin("##watermark", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

			ImGui::Text(ss.str().c_str());

			ImGui::End();

		}

		//auto _rotation = CGlobalData::Get().LocalController->RotationInput();

		//std::cout << "Pitch : " << _rotation.Pitch << " Yaw : " << _rotation.Yaw << " Roll : " << _rotation.Roll << "\n";

		if (CGlobalData::Get().PlayerCamera.Valid)
		{


			CGlobalData::Get().PlayerSync.lock();

			for (const auto& Player : CGlobalData::Get().Players)
			{

				bool Visible = Player.Mesh->IsVisible();

				if (Config::VisibleOnlyEsp && !Visible)
					continue;

				Vector3 PlayerHead = Player.Mesh->GetBonePos(68);
				float Distance = CGlobalData::Get().PlayerCamera.Location.Distance(PlayerHead) / 100.f;

				if (Distance > Config::MaxEspDistance)
					continue;

				PlayerHead.z += 15.f;
				Vector2 PlayerHeadScreen;

				if (!W2S(PlayerHead, PlayerHeadScreen))
					continue;

				Vector3 PlayerFoot = Player.Mesh->GetBonePos(0);
				Vector2 PlayerFootScreen;

				if (!W2S(PlayerFoot, PlayerFootScreen))
					continue;

				int BoxX;
				int BoxY;
				int BoxWidth;
				int BoxHeight;

				if (Config::BoxesUseBounds)
				{
					UnrealEngine::FBoxSphereBounds PlayerBounds = Memory.Read<UnrealEngine::FBoxSphereBounds>((uintptr_t)Player.Mesh + 0x7c8);
					Vector3 PlayerOrigin = Player.Pawn->RelativeLocation();
					Vector3 BoundsMin = PlayerOrigin - PlayerBounds.BoxExtent;
					Vector3 BoundsMax = PlayerOrigin + PlayerBounds.BoxExtent;
					Vector2 BoundsMinScreen;
					Vector2 BoundsMaxScreen;
					W2S(BoundsMin, BoundsMinScreen);
					W2S(BoundsMax, BoundsMaxScreen);
					BoxX = BoundsMinScreen.x;
					BoxY = BoundsMaxScreen.y;
					BoxWidth = BoundsMaxScreen.x - BoundsMinScreen.x;
					BoxHeight = BoundsMinScreen.y - BoundsMaxScreen.y;
				}
				else 
				{
					int height = PlayerHeadScreen.y - PlayerFootScreen.y;
					int width = height / 4;

					float Entity_x = PlayerFootScreen.x - width;
					float Entity_y = PlayerFootScreen.y;
					float Entity_w = height / 2;
					BoxX = Entity_x;
					BoxY = Entity_y;
					BoxWidth = Entity_w;
					BoxHeight = height;
				}

				if (Config::EnableBoxEsp && Config::BoxEspStyle == 1)
				{

					UnrealEngine::FBoxSphereBounds PlayerBounds = Memory.Read<UnrealEngine::FBoxSphereBounds>((uintptr_t)Player.Mesh + 0x7c8);
					Vector3 PlayerOrigin = Player.Pawn->RelativeLocation();

					auto v000 = PlayerOrigin - PlayerBounds.BoxExtent;
					auto v111 = PlayerOrigin + PlayerBounds.BoxExtent;

					auto w1 = ProjectWorldToScreen(Vector3(v000.x, v000.y, v000.z));
					auto w2 = ProjectWorldToScreen(Vector3(v111.x, v111.y, v111.z));

					// top box
					auto v010 = Vector3(v000.x, v111.y, v000.z);
					auto v110 = Vector3(v111.x, v111.y, v000.z);
					auto v011 = Vector3(v000.x, v111.y, v111.z);

					// bottom box
					auto v101 = Vector3(v111.x, v000.y, v111.z);
					auto v100 = Vector3(v111.x, v000.y, v000.z);
					auto v001 = Vector3(v000.x, v000.y, v111.z);

					auto s1 = ProjectWorldToScreen(v010);
					auto s2 = ProjectWorldToScreen(v110);
					auto s3 = ProjectWorldToScreen(v011);
					auto s4 = ProjectWorldToScreen(v101);
					auto s5 = ProjectWorldToScreen(v100);
					auto s6 = ProjectWorldToScreen(v001);

					ImColor Color = Visible ? Config::BoxEspColorVisible.c() : Config::BoxEspColorInVisible.c();

					if (s1.x || s1.y) {
						ImRenderer->DrawLineEx(w1, s6, Color);
						ImRenderer->DrawLineEx(w1, s5, Color);
						ImRenderer->DrawLineEx(s4, s5, Color);
						ImRenderer->DrawLineEx(s4, s6, Color);

						ImRenderer->DrawLineEx(s1, s3, Color);
						ImRenderer->DrawLineEx(s1, s2, Color);
						ImRenderer->DrawLineEx(w2, s2, Color);
						ImRenderer->DrawLineEx(w2, s3, Color);

						ImRenderer->DrawLineEx(s6, s3, Color);
						ImRenderer->DrawLineEx(w1, s1, Color);
						ImRenderer->DrawLineEx(s4, w2, Color);
						ImRenderer->DrawLineEx(s5, s2, Color);
					}
				}

				if (Config::EnableBoxEsp && Config::BoxEspStyle == 0)
					DrawBox(ImVec2(BoxX, BoxY), ImVec2(BoxWidth, BoxHeight), Visible ? Config::BoxEspColorVisible.c() : Config::BoxEspColorInVisible.c());

				if (Config::EnableBoxEsp && Config::BoxEspStyle == 2)
					ImRenderer->DrawCornerBox(BoxX, BoxY, BoxWidth, BoxHeight, Visible ? Config::BoxEspColorVisible.c() : Config::BoxEspColorInVisible.c());

				if (Config::EnableBoxEsp && Config::BoxEspStyle == 3)
					ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(BoxX, BoxY), ImVec2(BoxX + BoxWidth, BoxY + BoxHeight), Visible ? Config::BoxEspColorVisible.c() : Config::BoxEspColorInVisible.c());

				std::string RealPlayerName = Config::EnableNameEsp ? Player.Name : "";

				if (Config::EnableDistanceEsp)
					RealPlayerName += " (" + std::to_string(static_cast<int>(Distance)) + "M)";

				if (Config::EnableConsoleEsp && Player.Platform != "")
					RealPlayerName += " (" + Player.Platform + ")";

				ImRenderer->DrawTextGui(RealPlayerName, ImVec2(PlayerHeadScreen.x, PlayerHeadScreen.y - 14.f), 12.f, Visible ? Config::NameEspColorVisible.c() : Config::NameEspColorInVisible.c(), true, nullptr);

				if (Config::EnableCurrentWeaponEsp)
					ImRenderer->DrawTextGui(Player.WeaponName, ImVec2(PlayerHeadScreen.x, PlayerFootScreen.y + 2.f), 12.f, Visible ? Config::CurrentWeaponEspColorVisible.c() : Config::CurrentWeaponEspColorInVisible.c(), true, nullptr);

				if (Config::EnableSnaplines)
				{

					int Height = c_overlay->m_pHeight;
					int Width = c_overlay->m_pWidth;

					ImU32 Color = Visible ? Config::SnaplinesColorVisible.c() : Config::SnaplinesColorInVisible.c();

					switch (Config::SnaplineStart)
					{

					case 0:
						ImRenderer->DrawLineEx(ImVec2(Width / 2, 0), ImVec2(PlayerHeadScreen.x, PlayerHeadScreen.y), Color);
						break;
					case 1:
						ImRenderer->DrawLineEx(ImVec2(Width / 2, Height), ImVec2(PlayerHeadScreen.x, PlayerHeadScreen.y), Color);
						break;
					case 2:
						ImRenderer->DrawLineEx(ImVec2(Width / 2, Height / 2), ImVec2(PlayerHeadScreen.x, PlayerHeadScreen.y), Color);
						break;
					case 3:
						ImRenderer->DrawLineEx(ImVec2(Width / 2, 0), ImVec2(PlayerHeadScreen.x, PlayerHeadScreen.y), Color);
						break;

					}

				}

				if (Config::EnableSkeletonEsp)
				{

					ImColor Color = Visible ? Config::SkeletonEspColorVisible.x() : Config::SkeletonEspColorInVisible.x();

					#define B(x) Player.Mesh->GetBonePos(x)
					#define L(from, to) ImRenderer->DrawLineEx(from, to, Color, 1.f)

					const std::array <int, 3> Torso{ 2, 67, 7 };

					const std::array <int, 3> LeftLeg{ 78, 79, 80 };
					const std::array <int, 3> RightLeg{ 71, 72, 73 };

					const std::array <int, 3> LeftArm{ 35, 10, 34 };
					const std::array <int, 3> RightArm{ 64, 39, 62 };

					auto TorsoOne = B(Torso[0]);
					auto TorsoTwo = B(Torso[1]);
					auto TorsoThree = B(Torso[2]);

					auto TorsoOneScreen = ProjectWorldToScreen(TorsoOne);
					auto TorsoTwoScreen = ProjectWorldToScreen(TorsoTwo);

					L(TorsoOneScreen, TorsoTwoScreen);

					auto LeftLegOne = B(LeftLeg[0]);
					auto LeftLegTwo = B(LeftLeg[1]);
					auto LeftLegThree = B(LeftLeg[2]);

					auto LeftLegOneScreen = ProjectWorldToScreen(LeftLegOne);
					auto LeftLegTwoScreen = ProjectWorldToScreen(LeftLegTwo);
					auto LeftLegThreeScreen = ProjectWorldToScreen(LeftLegThree);

					L(TorsoOneScreen, LeftLegOneScreen);
					L(LeftLegOneScreen, LeftLegTwoScreen);
					L(LeftLegTwoScreen, LeftLegThreeScreen);

					auto RightLegOne = B(RightLeg[0]);
					auto RightLegTwo = B(RightLeg[1]);
					auto RightLegThree = B(RightLeg[2]);

					auto RightLegOneScreen = ProjectWorldToScreen(RightLegOne);
					auto RightLegTwoScreen = ProjectWorldToScreen(RightLegTwo);
					auto RightLegThreeScreen = ProjectWorldToScreen(RightLegThree);

					L(TorsoOneScreen, RightLegOneScreen);
					L(RightLegOneScreen, RightLegTwoScreen);
					L(RightLegTwoScreen, RightLegThreeScreen);

					auto LeftArmOne = B(LeftArm[0]);
					auto LeftArmTwo = B(LeftArm[1]);
					auto LeftArmThree = B(LeftArm[2]);

					L(ProjectWorldToScreen(TorsoThree), ProjectWorldToScreen(LeftArmOne));
					L(ProjectWorldToScreen(LeftArmOne), ProjectWorldToScreen(LeftArmTwo));
					L(ProjectWorldToScreen(LeftArmTwo), ProjectWorldToScreen(LeftArmThree));

					auto RightArmOne = B(RightArm[0]);
					auto RightArmTwo = B(RightArm[1]);
					auto RightArmThree = B(RightArm[2]);

					L(ProjectWorldToScreen(TorsoThree), ProjectWorldToScreen(RightArmOne));
					L(ProjectWorldToScreen(RightArmOne), ProjectWorldToScreen(RightArmTwo));
					L(ProjectWorldToScreen(RightArmTwo), ProjectWorldToScreen(RightArmThree));

				}

			}

			CAimbot::Get().Run(c_overlay->m_pWidth, c_overlay->m_pHeight);

			CGlobalData::Get().PlayerSync.unlock();

		}

		if (Config::ShowFOV)
			ImRenderer->DrawCircle(ImVec2(c_overlay->m_pWidth / 2, c_overlay->m_pHeight / 2), Config::AimbotFOV, Config::FOVColor.x());

		if (Config::EnableCrosshair)
		{

			ImVec2 Screen(c_overlay->m_pWidth, c_overlay->m_pHeight);
			const ImVec2 Middle(Screen.x / 2, Screen.y / 2);

			switch (Config::CrosshairStyle)
			{
			case 0:
			{
				ImRenderer->DrawLineEx(Middle, Middle + ImVec2(0, 6), Config::CrosshairColor.x());
				ImRenderer->DrawLineEx(Middle, Middle - ImVec2(0, 5), Config::CrosshairColor.x());
				ImRenderer->DrawLineEx(Middle, Middle + ImVec2(6, 0), Config::CrosshairColor.x());
				ImRenderer->DrawLineEx(Middle, Middle - ImVec2(5, 0), Config::CrosshairColor.x());
				break;
			}
			case 1:
			{
				ImRenderer->DrawCircleFilled(Middle, 2.f, Config::CrosshairColor.x());
				break;
			}
			case 2:
			{
				if (rotation > 89) rotation = 0;
				++rotation;

				int a = (int)(Screen.y / 2 / 30);
				float gamma = atan(1);
				int Drehungswinkel = rotation;

				ColorChange();

				for (int i = 0; i < 4; i++)
				{

					std::vector <int> p;
					p.push_back(a * sin(GRD_TO_BOG(Drehungswinkel + (i * 90))));									//p[0]		p0_A.x
					p.push_back(a * cos(GRD_TO_BOG(Drehungswinkel + (i * 90))));									//p[1]		p0_A.y
					p.push_back((a / cos(gamma)) * sin(GRD_TO_BOG(Drehungswinkel + (i * 90) + BOG_TO_GRD(gamma))));	//p[2]		p0_B.x
					p.push_back((a / cos(gamma)) * cos(GRD_TO_BOG(Drehungswinkel + (i * 90) + BOG_TO_GRD(gamma))));	//p[3]		p0_B.y

					ImRenderer->DrawLineEx(ImVec2(Middle.x, Middle.y), ImVec2(Middle.x + p[0], Middle.y - p[1]), rainbow_nazi.x());
					ImRenderer->DrawLineEx(ImVec2(Middle.x + p[0], Middle.y - p[1]), ImVec2(Middle.x + p[2], Middle.y - p[3]), rainbow_nazi.x());

				}
				break;
			}
			}

		}

		if (Config::KeybindList)
		{

			ImGui::Begin("keybinds", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::Text("keybinds");
			
			std::vector <std::string> binds{};

			if (Config::AimbotKey.enabled)
				binds.push_back(Config::AimbotKey.get_name() + " ~ " + Config::AimbotKey.get_type());

			if (Config::SpinbotKey.enabled)
				binds.push_back(Config::SpinbotKey.get_name() + " ~ " + Config::SpinbotKey.get_type());

			for (const auto& str : binds)
				ImGui::Text(str.c_str());

			ImGui::End();

		}

		ImGui::EndFrame();
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			g_pd3dDevice->Reset(&g_d3dpp);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

	}
	

	return 1;

}

struct injection_data
{

	char username[255];
	char expiry[255];
	uintptr_t key;
	unsigned int size;
	char process_name[255];

};

#ifndef _DEBUG
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{

	if (fdwReason == DLL_PROCESS_ATTACH)
	{

		injection_data* data = reinterpret_cast<injection_data*>(lpvReserved);

		if (!lpvReserved)
			reinterpret_cast<int>(nullptr)();

		if (!data)
			reinterpret_cast<int>(nullptr)();

		if (data->size != 0x4500)
			reinterpret_cast<int>(nullptr)();

		if (strcmp(data->process_name, "EpicWebHelper.exe"))
			reinterpret_cast<int>(nullptr)();

		Config::Username = data->username;

		DisableThreadLibraryCalls(hinstDLL);
		CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(main_thread), NULL, NULL, NULL);

	}

	return TRUE;

}
#else
int main()
{
	main_thread();
}
#endif