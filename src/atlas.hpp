#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>
#include <algorithm>
#include <px/graphics/texture.hpp>
#include <px/math/vec.hpp>
#include <vector>

struct TextureReference
{
    px::Vec2i location;
    px::Vec2i size;
};

struct TextureAtlas
{
    std::vector<TextureReference> references;
    px::Texture atlasTexture;
};

inline TextureAtlas createTextureAtlas(std::vector<px::Texture> &textures, int maxWidth)
{
    std::sort(textures.begin(), textures.end(),
              [](const px::Texture &a, const px::Texture &b) { return a.getSize().y > b.getSize().y; });

    std::vector<TextureReference> references(textures.size());
    px::Vec2i sizeNeededForAtlas;
    px::Vec2i currentPos;
    int tallestTexture = 0;
    for (size_t i = 0; i < textures.size(); ++i)
    {
        const px::Texture &current = textures[i];
        px::Vec2i size             = current.getSize().toInt();

        if (currentPos.x + size.x > maxWidth)
        {
            currentPos.x = 0;
            currentPos.y += tallestTexture;
            tallestTexture = 0;
        }

        if (size.y > tallestTexture)
        {
            tallestTexture = size.y;
        }

        references[i]        = {currentPos, size};
        sizeNeededForAtlas.x = std::max(currentPos.x + size.x, sizeNeededForAtlas.x);
        sizeNeededForAtlas.y = std::max(currentPos.y + size.y, sizeNeededForAtlas.y);

        currentPos.x += size.x;
    }

    px::Texture atlasTexture(sizeNeededForAtlas.toUnsignedInt());

    for (size_t i = 0; i < textures.size(); ++i)
    {
        TextureReference reference = references[i];
        const px::Texture &current = textures[i];
        px::Vec2i size             = current.getSize().toInt();

        for (int y = 0; y < size.y; ++y)
        {
            for (int x = 0; x < size.x; ++x)
            {
                px::Vec2i pos      = {x, y};
                px::Vec2i atlasPos = reference.location + pos;

                atlasTexture.setPixel(atlasPos.toUnsignedInt(), current.getPixel(pos.toUnsignedInt()));
            }
        }
    }

    return {references, atlasTexture};
}
