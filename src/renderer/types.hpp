#pragma once

#include <cstdint>
#include <px/math/vec.hpp>
#include <variant>

struct TextureDrawInfo
{
    uint64_t id;
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
