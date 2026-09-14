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

struct GPUTextureObject
{
    float atlasPositionNormal[2];
    float atlasSizeNormal[2];
    float position[2];
    float size[2];
    float origin[2];
    float angle;
    float _pad0;
    float cropTopLeft[2];
    float cropBottomRight[2];
};

struct GPURectangleObject
{
    float position[2];
    float size[2];
    float origin[2];
    float angle;
    float _pad0;
    float color[4];
};

struct GPUEllipseObject
{
    float position[2];
    float size[2];
    float origin[2];
    float angle;
    float _pad0;
    float color[4];
};

struct GPUDrawObject
{
    int type;
    int index;
    float opacity;
    int filter;
};
