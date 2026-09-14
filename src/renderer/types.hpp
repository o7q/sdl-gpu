#pragma once

#include <px/math/vec.hpp>
#include <string>
#include <variant>

struct TextureDrawInfo
{
    std::string name = "";
    px::Vec2f position;
    px::Vec2f size;
    float angle;
};

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

using DrawInfoType = std::variant<TextureDrawInfo, RectangleDrawInfo, EllipseDrawInfo>;

struct DrawInfo
{
    int index = 0;
    DrawInfoType info;
};
