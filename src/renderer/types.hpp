#pragma once

#include <variant>

struct Vec2
{
    float x = 0.0f, y = 0.0f;
};

struct RectangleDrawInfo
{
    Vec2 position;
    Vec2 size;
    float angle;
};

struct CircleDrawInfo
{
    Vec2 position;
    float radius;
};

using DrawInfoType = std::variant<RectangleDrawInfo, CircleDrawInfo>;

struct DrawInfo
{
    int index = 0;
    DrawInfoType info;
};
