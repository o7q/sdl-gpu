#pragma once

#include <cstdint>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <px/math/angle.hpp>
#include <px/math/vec.hpp>
#include <px/variant_bp.hpp>
#include <set>
#include <variant>

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
    px::Color color        = {255, 255, 255, 255};
    bool fill              = true;
    float outlineThickness = 1.0f;
    float opacity          = 1.0f;
    int filter             = 0;
};

struct EllipseDrawInfo
{
    px::Vec2f position;
    px::Vec2f size;
    px::Vec2Opt_f origin;
    px::Angle angle;
    px::Color color        = {255, 255, 255, 255};
    bool fill              = true;
    float outlineThickness = 1.0f;
    float opacity          = 1.0f;
    int filter             = 0;
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
};

class DrawLayer
{
  public:
    void addSprite(DrawInfoType info, int zIndex)
    {
        DrawInfo info_;
        info_.object = info;
        info_.zIndex = zIndex;
        m_objects.push_back(info_);
    }

    void sort()
    {
        std::sort(m_objects.begin(), m_objects.end(),
                  [](const DrawInfo &a, const DrawInfo &b) { return a.zIndex < b.zIndex; });
    }

    const std::vector<DrawInfo> &getObjects() const { return m_objects; }

  private:
    std::vector<DrawInfo> m_objects;
};

struct DrawLayerData
{
    DrawLayer layer;
    DrawLayerBlendMode blendMode = DrawLayerBlendMode::AlphaOver;
    int zIndex                   = 0;
};

class DrawCommand
{
  public:
    void addLayer(DrawLayer &layer, int zIndex = 0,
                  DrawLayerBlendMode blendMode = DrawLayerBlendMode::AlphaOver)
    {
        DrawLayerData data;
        data.layer     = std::move(layer);
        data.zIndex    = zIndex;
        data.blendMode = blendMode;
        m_layers.push_back(data);
    }

    void sort()
    {
        std::sort(m_layers.begin(), m_layers.end(),
                  [](const DrawLayerData &a, const DrawLayerData &b) { return a.zIndex < b.zIndex; });
    }

    std::vector<DrawLayerData> &getLayers() { return m_layers; }

  private:
    std::vector<DrawLayerData> m_layers;
};
