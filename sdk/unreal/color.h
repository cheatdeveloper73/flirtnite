#pragma once

#include "../includes.h"

struct Color
{
	float r, g, b, a;
    Color()
    {
        r = 1.f, g = 1.f, b = 1.f, a = 1.f;
    }
    Color(float _r, float _g, float _b, float _a)
    {
        this->r = _r;
        this->g = _g;
        this->b = _b;
        this->a = _a;
    }
    ImVec4 x()
    {
        return ImVec4(r, g, b, a);
    }
    ImU32 c()
    {
        return IM_COL32(r * 255.f, g * 255.f, b * 255.f, a * 255.f);
    }
    ImU32 c(int m)
    {
        return IM_COL32((r * 255.f) - m, (g * 255.f) - m, (b * 255.f) - m, (a * 255.f) - m);
    }
};