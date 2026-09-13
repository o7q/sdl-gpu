#pragma once

#include <px/math/vec.hpp>
#include <variant>

struct RectangleDrawInfo
{
    px::Vec2f position;
    px::Vec2f size;
    float angle;
};

struct EllipseDrawInfo
{
    px::Vec2f position;
    px::Vec2f size;
    float angle;
};

using DrawInfoType = std::variant<RectangleDrawInfo, EllipseDrawInfo>;

struct DrawInfo
{
    int index = 0;
    DrawInfoType info;
};
