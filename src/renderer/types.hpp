#pragma once

#include <cstdint>
#include <px/graphics/color.hpp>
#include <px/math/angle.hpp>
#include <px/math/vec.hpp>
#include <variant>

struct TextureDrawInfo
{
    uint64_t id;
    px::Vec2f position;
    px::Vec2f scale;
    px::Vec2Opt_f origin;
    px::Angle angle;
    px::Vec2f cropTopLeft;
    px::Vec2f cropBottomRight;
    float opacity = 1.0f;
    int filter    = 0;
};

struct RectangleDrawInfo
{
    px::Vec2f position;
    px::Vec2f size;
    px::Vec2Opt_f origin;
    px::Angle angle;
    px::Color color;
    float opacity = 1.0f;
    int filter    = 0;
};

struct EllipseDrawInfo
{
    px::Vec2f position;
    px::Vec2f size;
    px::Vec2Opt_f origin;
    px::Angle angle;
    px::Color color;
    float opacity = 1.0f;
    int filter    = 0;
};

using DrawInfoType = std::variant<TextureDrawInfo, RectangleDrawInfo, EllipseDrawInfo>;

struct DrawInfo
{
    int index = 0;
    DrawInfoType info;
};
