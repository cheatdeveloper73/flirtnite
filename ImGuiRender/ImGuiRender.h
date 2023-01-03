#pragma once
#include "../ImGui/imgui.h"
#include <string>

class GRenderer
{
public:
	GRenderer();
	void Initialize();
	float DrawTextGui(const std::string& text, const ImVec2& pos, float size, const ImColor& color, bool center, ImFont* fn);
	void DrawLineEx(const ImVec2& from, const ImVec2& to, const ImColor& color, float thickness = 1.0f);
	void DrawCircle(const ImVec2& position, float radius, const ImColor& color, float thickness = 1.0f);
	void DrawCircleFilled(const ImVec2& position, float radius, const ImColor& color);
	void DrawFilledRect(const ImVec2& v1, const ImVec2& v2, const ImColor& color);
	void RenderString(const ImVec2& pos, const ImColor& color, bool Flag, const CHAR *fmt, ...);
	void Border(float x, float y, float w, float h, const ImColor& color);
	void DrawCornerBox(float x, float y, float w, float h, const ImColor& color);
	void DrawBox(float x, float y, float w, float h, const ImColor& color, float rounding = 0.f);
	void DrawHealth(int health, ImVec2 pos, ImVec2 size);
	void DrawName(std::string name, ImVec2 pos, ImVec2 size);
	void DrawName(std::string name, ImVec2 pos, ImVec2 size, ImColor color);

	ImFont* GetNormalFont()
	{
		return m_pNormal;
	}
	ImFont* GetBoldFont()
	{
		return m_pBold;
	}

	ImFont* getsmallfonts()
	{
		return m_smallfonts;
	}

	ImFont* geticon()
	{
		return m_pIconFont;
	}

private:
	ImFont* m_pFont = NULL;
	ImFont* m_pNormal = NULL;
	ImFont* m_pBold = NULL;
	ImFont* m_smallfonts = NULL;
	ImFont* m_pIconFont = NULL;
};

extern GRenderer* ImRenderer;
