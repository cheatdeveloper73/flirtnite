#include "menu.h"

#include "../sdk/config.h"

struct tab
{
    const char* tab_name;
    int tab_id;
};

std::vector<tab> tabs{ {"aimbot", 0}, {"player esp", 1}, {"loot esp", 2}, {"miscellaneous", 3}, {"configs", 4} };
int current_tab = 0;

namespace ImGui
{
    bool Keybind(CKeybind* keybind, const ImVec2& size_arg = ImVec2(0, 0), bool clicked = false, ImGuiButtonFlags flags = 0)
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;
        //SetCursorPosX(window->Size.x - 14 - size_arg.x);
        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(keybind->get_name().c_str());
        const ImVec2 label_size = ImGui::CalcTextSize(keybind->get_name().c_str(), NULL, true);

        ImVec2 pos = window->DC.CursorPos;
        if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
            pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
        ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        const ImRect bb(pos, pos + size);
        ItemSize(size, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        if (g.CurrentItemFlags & ImGuiItemFlags_ButtonRepeat)
            flags |= ImGuiButtonFlags_Repeat;
        bool hovered, held;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

        bool value_changed = false;
        int key = keybind->key;

        auto io = ImGui::GetIO();

        std::string name = keybind->get_key_name();

        if (keybind->waiting_for_input)
            name = "waiting";

        if (ImGui::GetIO().MouseClicked[0] && hovered)
        {

            if (g.ActiveId == id)
            {

                keybind->waiting_for_input = true;

            }

        }
        else if (ImGui::GetIO().MouseClicked[1] && hovered) {
            OpenPopup(keybind->get_name().c_str());
        }
        else if (ImGui::GetIO().MouseClicked[0] && !hovered) {
            if (g.ActiveId == id)
                ImGui::ClearActiveID();
        }

        if (keybind->waiting_for_input)
            if (keybind->set_key())
            {
                ImGui::ClearActiveID();
                keybind->waiting_for_input = false;
            }

        // Render
        ImVec4 textcolor = ImLerp(ImVec4(201 / 255.f, 204 / 255.f, 210 / 255.f, 1.f), ImVec4(1.0f, 1.0f, 1.0f, 1.f), 1.f);

        window->DrawList->AddRectFilled(bb.Min, bb.Max, ImColor(33 / 255.0f, 33 / 255.0f, 33 / 255.0f, 1.f));
        window->DrawList->AddRect(bb.Min, bb.Max, ImColor(0.f, 0.f, 0.f, 1.f));

        window->DrawList->AddText(bb.Min + ImVec2(size_arg.x / 2 - CalcTextSize(name.c_str()).x / 2, size_arg.y / 2 - CalcTextSize(name.c_str()).y / 2), ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Text)), name.c_str());

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
        SetNextWindowPos(pos + ImVec2(0, size_arg.y - 1));
        SetNextWindowSize(ImVec2(size_arg.x, 47 * 1.f));

        {

            if (BeginPopup(keybind->get_name().c_str(), window_flags)) {
                PushStyleVar(ImGuiStyleVar_Alpha, 1.f); {
                    SetCursorPos(ImVec2(7, 2)); {
                        BeginGroup(); {
                            if (Selectable("always on", keybind->type == CKeybind::ALWAYS)) keybind->type = CKeybind::ALWAYS;
                            if (Selectable("hold", keybind->type == CKeybind::HOLD)) keybind->type = CKeybind::HOLD;
                            if (Selectable("toggle", keybind->type == CKeybind::TOGGLE)) keybind->type = CKeybind::TOGGLE;
                        } EndGroup();
                    }
                } PopStyleVar();
                EndPopup();
            }
        }

        return pressed;
    }
}

void AlignForWidth(float width, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();
    float avail = ImGui::GetContentRegionAvail().x;
    float off = (avail - width) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
}

void CMenu::Render()
{

    ImGui::SetNextWindowPos(ImVec2(window_pos.x, window_pos.y), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(window_size.x, window_size.y), ImGuiCond_Once);
    ImGui::SetNextWindowBgAlpha(1.0f);

    ImGui::PushStyleColor(ImGuiCol_CheckMark, AccentColor.x());
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, AccentColor.x());
    ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, AccentColor.x());

    ImGui::Begin("Stinkcheat", NULL, window_flags);
    {

        auto draw = ImGui::GetForegroundDrawList();
        auto size = ImGui::GetWindowSize();
        auto pos = ImGui::GetWindowPos();

        draw->AddRectFilled(pos + ImVec2(2, 2), pos + ImVec2(size.x - 2, 1), IM_COL32(74, 74, 74, 255));
        draw->AddRectFilled(pos + ImVec2(2, 3), pos + ImVec2(size.x - 2, 2), AccentColor.c());
        draw->AddRectFilled(pos + ImVec2(2, 4), pos + ImVec2(size.x - 2, 3), AccentColor.c());
        draw->AddRectFilled(pos + ImVec2(2, 5), pos + ImVec2(size.x - 2, 4), IM_COL32(74, 74, 74, 255));

        ImGui::BeginChild("Tabs", ImVec2(85, size.y - 15), true);

        ImGui::PushFontShadow(IM_COL32(0, 0, 0, 255));

        ImGui::PushStyleColor(ImGuiCol_Text, AccentColor.x());
        AlignForWidth(ImGui::CalcTextSize("stinkcheat").x);
        ImGui::Text("stinkcheat");
        ImGui::PopStyleColor();

        ImGui::Spacing();

        for (const auto& tab : tabs)
        {
            bool tmp = current_tab == tab.tab_id;
            if (ImGui::Selectable(tab.tab_name, &tmp)) current_tab = tab.tab_id;
        }

        ImGui::PopFontShadow();

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("Content", ImVec2((size.x - 85) - 26, size.y - 15), true);

        switch (current_tab)
        {

        case 0: // aim
        {

            ImGui::Checkbox("enable aimbot", &Config::EnableAimbot);
            ImGui::Text("aimbot key");
            ImGui::SameLine();
            ImGui::Keybind(&Config::AimbotKey, ImVec2(60, 16));
            ImGui::Combo("aimbot hitbox", &Config::AimbotHitbox, "head\0chest\0nearest");
            ImGui::SliderFloat("aimbot fov", &Config::AimbotFOV, 0.f, 600.f, "%.1f");
            ImGui::Checkbox("enable smoothing", &Config::EnableAimbotSmoothing);
            if (Config::EnableAimbotSmoothing)
                ImGui::SliderFloat("smoothing", &Config::AimbotSmoothing, 0.f, 100.f, "%.1f%%");
        
            ImGui::Checkbox("draw fov", &Config::ShowFOV);
            ImGui::SameLine();
            ImGui::ColorEdit4("fov color", &Config::FOVColor.r);

            ImGui::Checkbox("only visible", &Config::VisibleOnlyAimbot);
            ImGui::SliderFloat("max aim distance", &Config::MaxAimbotDistance, 10.f, 300.f, "%1.fM");

            break;

        }
        case 1: // player esp
        {

            ImGui::Checkbox("enable name esp", &Config::EnableNameEsp);
            ImGui::SameLine();
            ImGui::ColorEdit4("name color visible", &Config::NameEspColorVisible.r);
            ImGui::SameLine();
            ImGui::ColorEdit4("name color invisible", &Config::NameEspColorInVisible.r);

            ImGui::Checkbox("enable box esp", &Config::EnableBoxEsp);
            ImGui::SameLine();
            ImGui::ColorEdit4("box color visible", &Config::BoxEspColorVisible.r);
            ImGui::SameLine();
            ImGui::ColorEdit4("box color invisible", &Config::BoxEspColorInVisible.r);
            ImGui::Combo("box esp style", &Config::BoxEspStyle, "standard\0world\0corners\0filled");

            ImGui::Checkbox("enable skeleton esp", &Config::EnableSkeletonEsp);
            ImGui::SameLine();
            ImGui::ColorEdit4("skeleton color visible", &Config::SkeletonEspColorVisible.r);
            ImGui::SameLine();
            ImGui::ColorEdit4("skeleton color invisible", &Config::SkeletonEspColorInVisible.r);

            ImGui::Checkbox("enable weapon esp", &Config::EnableCurrentWeaponEsp);
            ImGui::SameLine();
            ImGui::ColorEdit4("weapon color visible", &Config::CurrentWeaponEspColorVisible.r);
            ImGui::SameLine();
            ImGui::ColorEdit4("weapon color invisible", &Config::CurrentWeaponEspColorInVisible.r);

            ImGui::Checkbox("enable snaplines", &Config::EnableSnaplines);
            ImGui::SameLine();
            ImGui::ColorEdit4("line color visible", &Config::SnaplinesColorVisible.r);
            ImGui::SameLine();
            ImGui::ColorEdit4("line color invisible", &Config::SnaplinesColorInVisible.r);

            ImGui::Combo("snapline start", &Config::SnaplineStart, "top\0bottom\0middle\0");

            ImGui::Checkbox("enable platform esp", &Config::EnableConsoleEsp);
            ImGui::Checkbox("enable distance esp", &Config::EnableDistanceEsp);
            ImGui::SliderFloat("max esp distance", &Config::MaxEspDistance, 10.f, 300.f, "%.1fM");
            ImGui::Checkbox("show teammates", &Config::EnableTeamCheck);
            ImGui::Checkbox("only visible", &Config::VisibleOnlyEsp);
            ImGui::Checkbox("show bots", &Config::EnableBotCheck);

            break;

        }
        case 2: // loot esp
        {
            break;
        }
        case 3: // misc
        {

            ImGui::Text("menu accent color");
            ImGui::SameLine();
            ImGui::ColorEdit4("menu accent", &AccentColor.r);
            ImGui::Checkbox("enable spinbot", &Config::EnableSpinbot);
            ImGui::Text("spinbot key");
            ImGui::SameLine();
            ImGui::Keybind(&Config::SpinbotKey, ImVec2(60, 16));
            ImGui::Checkbox("enable no-recoil", &Config::EnableNoRecoil);
            ImGui::Checkbox("enable no-spread", &Config::EnableNoSpread);
            ImGui::Checkbox("enable crosshair", &Config::EnableCrosshair);
            ImGui::SameLine();
            ImGui::ColorEdit4("crosshair color", &Config::CrosshairColor.r);
            ImGui::Combo("crosshair style", &Config::CrosshairStyle, "standard\0circle\0swastika");
            ImGui::Checkbox("enable keybind list", &Config::KeybindList);
            ImGui::Checkbox("enable instant weapon switch", &Config::EnableInstantWeaponDeploy);

            break;

        }
        case 4: // configs
        {
            break;
        }

        }

        ImGui::EndChild();

    }
    ImGui::End();

    ImGui::PopStyleColor(3);

}

bool CMenu::Initialize()
{

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImFontConfig cfg;
    cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Monochrome | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting;
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 11.f, &cfg);

	return true;

}