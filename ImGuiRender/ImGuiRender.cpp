#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "../ImGui/imgui.h"
#include "ImGuiRender.h"
#include "../ImGui/imgui_freetype.h"

GRenderer* ImRenderer = nullptr;

GRenderer::GRenderer()
{
}

void GRenderer::Initialize()
{

	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig FontConfig{};

	ImFontConfig cfg;
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_MonoHinting;
	cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Monochrome;

	ImFontConfig imst;
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.TTF", 18.0f, &FontConfig, io.Fonts->GetGlyphRangesCyrillic());
	if (!m_pFont)
		m_pFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf", 22.f, &FontConfig, io.Fonts->GetGlyphRangesCyrillic());
	if (!m_pNormal)
		m_pNormal = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\arial.ttf", 12.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	if (!m_pBold)
		m_pBold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 14.f, &FontConfig, io.Fonts->GetGlyphRangesCyrillic());
	if (!m_smallfonts)
		m_pBold = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 6.f, &FontConfig, io.Fonts->GetGlyphRangesCyrillic());

}

void GRenderer::DrawHealth(int health, ImVec2 pos, ImVec2 size)
{
	float max_health = 100.f;
	float health_ratio = health / max_health;

	int offset = size.y - 2;
	offset -= (offset * health_ratio);

	ImColor color = ImColor(int(244 - (116 * health_ratio)), int(100 + (144 * health_ratio)), 66, int(220 * 1.f));

	DrawFilledRect(ImVec2(pos.x - 5, pos.y), ImVec2(4, size.x), ImColor(0, 0, 0, int(130 * 1.f)));
	DrawFilledRect(ImVec2(pos.x - 4, pos.y + 1 + offset), ImVec2(2, size.x - 2 - offset), color);
	if (health_ratio < 0.95)
	{

		DrawTextGui(std::to_string(health).c_str(), ImVec2(pos.x, pos.y + offset), 12.f, ImColor(255, 255, 255, int(200 * 1.f)), true, nullptr);

	}


}

void GRenderer::DrawName(std::string name, ImVec2 pos, ImVec2 size)
{

	DrawTextGui(name, ImVec2((pos.x), pos.y - 11), 12.f, ImColor(0, 0, 0, int((200 / 1.5f) * 1.f)), true, nullptr);
	DrawTextGui(name, ImVec2(pos.x, pos.y - 12), 12.f, ImColor(255, 255, 255, 255), true, nullptr);

}

void GRenderer::DrawName(std::string name, ImVec2 pos, ImVec2 size, ImColor color)
{

	//DrawTextGui(name, ImVec2((pos.x), pos.y - 11), 12.f, ImColor(0, 0, 0, int((200 / 1.5f) * 1.f)), true, nullptr);
	DrawTextGui(name, ImVec2(pos.x, pos.y - 12), 12.f, color, true, nullptr);

}

float GRenderer::DrawTextGui(const std::string& text, const ImVec2& pos, float size, const ImColor& color, bool center, ImFont* fn)
{
	ImDrawList* windowDrawList = ImGui::GetBackgroundDrawList();
	std::stringstream steam(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(steam, line))
	{
		ImVec2 textSize = m_pNormal->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
		if (center)
		{
			windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			//windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			//windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			//windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			windowDrawList->AddText(m_pNormal, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w)), line.c_str());
		}
		else
		{
			windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			//windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			//windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			//windowDrawList->AddText(m_pNormal, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, color.Value.w)), line.c_str());
			windowDrawList->AddText(m_pNormal, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w)), line.c_str());
		}
		y = pos.y + textSize.y * (i + 1);
		i++;
	}
	return y;
}

void GRenderer::RenderString(const ImVec2& pos, const ImColor& color, bool Flag, const CHAR *fmt, ...)
{
	char buf[1024] = { '\0' };
	va_list va_alist;
	va_start(va_alist, fmt);
	vsprintf(buf, fmt, va_alist);
	va_end(va_alist);
	DrawTextGui(buf, pos, 12.f, color, Flag, nullptr);
}

void GRenderer::DrawLineEx(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddLine(from, to, ImGui::GetColorU32(ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w)), thickness);
}

void GRenderer::DrawCircle(const ImVec2& position, float radius, const ImColor& color, float thickness)
{
	ImGui::GetBackgroundDrawList()->AddCircle(position, radius, ImGui::GetColorU32(ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w)), 30, thickness);
}

void GRenderer::DrawCircleFilled(const ImVec2& position, float radius, const ImColor& color)
{
	ImGui::GetBackgroundDrawList()->AddCircleFilled(position, radius, ImGui::GetColorU32(ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w)), 12);
}

void GRenderer::DrawFilledRect(const ImVec2& v1, const ImVec2& v2, const ImColor& color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(v1, ImVec2(v1.x + v2.x, v1.y + v2.y), color, 0, 0);
}

void GRenderer::Border(float x, float y, float w, float h, const ImColor& color)
{
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::GetColorU32(ImVec4(color.Value.x, color.Value.y, color.Value.z, color.Value.w)), 2, 0, 2);
}

void GRenderer::DrawCornerBox(float x, float y, float w, float h, const ImColor& color)
{
	float lineW = (w / 5);
	float lineH = (h / 6);
	float lineT = 1;

	DrawLineEx(ImVec2(x - lineT, y - lineT), ImVec2(x + lineW, y - lineT), ImColor(0, 0, 0, 255)); //top left
	DrawLineEx(ImVec2(x - lineT, y - lineT), ImVec2(x - lineT, y + lineH), ImColor(0, 0, 0, 255));
	DrawLineEx(ImVec2(x - lineT, y + h - lineH), ImVec2(x - lineT, y + h + lineT), ImColor(0, 0, 0, 255));
	DrawLineEx(ImVec2(x - lineT, y + h + lineT), ImVec2(x + lineW, y + h + lineT), ImColor(0, 0, 0, 255));
	DrawLineEx(ImVec2(x + w - lineW, y - lineT), ImVec2(x + w + lineT, y - lineT), ImColor(0, 0, 0, 255));
	DrawLineEx(ImVec2(x + w + lineT, y - lineT), ImVec2(x + w + lineT, y + lineH), ImColor(0, 0, 0, 255));
	DrawLineEx(ImVec2(x + w + lineT, y + h - lineH), ImVec2(x + w + lineT, y + h + lineT), ImColor(0, 0, 0, 255));
	DrawLineEx(ImVec2(x + w - lineW, y + h + lineT), ImVec2(x + w + lineT, y + h + lineT), ImColor(0, 0, 0, 255));

	//inline
	DrawLineEx(ImVec2(x, y), ImVec2(x, y + lineH), color);
	DrawLineEx(ImVec2(x, y), ImVec2(x + lineW, y), color);
	DrawLineEx(ImVec2(x + w - lineW, y), ImVec2(x + w, y), color);
	DrawLineEx(ImVec2(x + w, y), ImVec2(x + w, y + lineH), color);
	DrawLineEx(ImVec2(x, y + h - lineH), ImVec2(x, y + h), color);
	DrawLineEx(ImVec2(x, y + h), ImVec2(x + lineW, y + h), color);
	DrawLineEx(ImVec2(x + w - lineW, y + h), ImVec2(x + w, y + h), color);
	DrawLineEx(ImVec2(x + w, y + h - lineH), ImVec2(x + w, y + h), color);
}

void GRenderer::DrawBox(float x, float y, float w, float h, const ImColor& color, float round)
{

	ImDrawList* draw = ImGui::GetBackgroundDrawList();

	draw->AddRect(ImVec2(x, y), ImVec2(w, h), IM_COL32(255, 255, 255, 255));
	draw->AddRect(ImVec2(x - 1, y - 1), ImVec2(w + 1, h + 1), IM_COL32(0, 0, 0, 130));
	draw->AddRect(ImVec2(x + 1, y + 1), ImVec2(w - 1, h - 1), IM_COL32(0, 0, 0, 130));

}
