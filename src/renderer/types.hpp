#pragma once

#include "renderer/gpu_types.hpp"
#include <cstdint>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <px/math/angle.hpp>
#include <px/math/vec.hpp>
#include <px/variant_bp.hpp>
#include <set>
#include <variant>
#include <vector>

using TextureType = std::variant<uint64_t, px::Texture>;

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

struct DrawInfo
{
    DrawInfoType object;
    int zIndex = 0;

    bool operator<(const DrawInfo &other) const { return zIndex < other.zIndex; }
};

class DrawLayer
{
  public:
    void addSprite(DrawInfoType info, int zIndex)
    {
        DrawInfo info_;
        info_.object = info;
        info_.zIndex = zIndex;
        m_objects.insert(info_);
    }

    const std::multiset<DrawInfo> &getObjects() const { return m_objects; }

  private:
    std::multiset<DrawInfo> m_objects;
};

struct DrawLayerData
{
    DrawLayer layer;
    DrawLayerBlendMode blendMode = DrawLayerBlendMode::AlphaOver;
    int zIndex                   = 0;

    bool operator<(const DrawLayerData &other) const { return zIndex < other.zIndex; }
};

class DrawCommand
{
  public:
    void addLayer(DrawLayer &layer, int zIndex = 0,
                  DrawLayerBlendMode blendMode = DrawLayerBlendMode::AlphaOver)
    {
        DrawLayerData data;
        data.layer     = layer;
        data.zIndex    = zIndex;
        data.blendMode = blendMode;
        m_layers.insert(data);
    }

    const std::multiset<DrawLayerData> &getLayers() const { return m_layers; }

  private:
    std::multiset<DrawLayerData> m_layers;
};
