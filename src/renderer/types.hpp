#pragma once

#include <cstdint>
#include <px/graphics/color.hpp>
#include <px/math/angle.hpp>
#include <px/math/vec.hpp>
#include <variant>
#include <vector>

struct TextureDrawInfo
{
    uint64_t id;
    px::Vec2f position;
    px::Vec2f scale = {1.0f, 1.0f};
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
    px::Color color = {255, 255, 255, 255};
    float opacity   = 1.0f;
    int filter      = 0;
};

struct EllipseDrawInfo
{
    px::Vec2f position;
    px::Vec2f size;
    px::Vec2Opt_f origin;
    px::Angle angle;
    px::Color color = {255, 255, 255, 255};
    float opacity   = 1.0f;
    int filter      = 0;
};

struct LineDrawInfo
{
    px::Vec2f a, b;
    float thickness = 2.0f;
    px::Color color = {255, 255, 255, 255};
    float opacity   = 1.0f;
    int filter      = 0;
};

using DrawInfoType = std::variant<TextureDrawInfo, RectangleDrawInfo, EllipseDrawInfo, LineDrawInfo>;

struct DrawInfo
{
    int zIndex = 0;
    DrawInfoType info;
};

enum class DrawLayerBlendMode
{
    Replace,
    AlphaOver,
    Add,
    Subtract,
    Multiply,
    Divide,
    And,
    Or,
    Xor,
    Lighten,
    Darken
};

struct DrawLayer
{
    int zIndex                   = 0;
    DrawLayerBlendMode blendMode = DrawLayerBlendMode::AlphaOver;
    std::vector<DrawInfo> sprites;
};
