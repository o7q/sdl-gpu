#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_surface.h>
#include <algorithm>
#include <px/graphics/texture.hpp>
#include <px/math/vec.hpp>
#include <string>
#include <unordered_map>
#include <vector>

struct NamedTexture
{
    std::string name = "";
    px::Texture texture;
};

class TextureAtlas
{
  private:
    struct TextureIndex
    {
        px::Vec2i location;
        px::Vec2i size;
    };

  public:
    TextureAtlas(std::vector<NamedTexture> &textures, int maxWidth)
    {
        std::sort(textures.begin(), textures.end(), [](const NamedTexture &a, const NamedTexture &b)
                  { return a.texture.getSize().y > b.texture.getSize().y; });

        px::Vec2i sizeNeededForAtlas;
        px::Vec2i currentPos;
        int tallestTexture = 0;
        for (const NamedTexture &texture : textures)
        {
            px::Vec2i size = texture.texture.getSize().toInt();

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

            m_indices[texture.name] = {currentPos, size};
            sizeNeededForAtlas.x    = std::max(currentPos.x + size.x, sizeNeededForAtlas.x);
            sizeNeededForAtlas.y    = std::max(currentPos.y + size.y, sizeNeededForAtlas.y);

            currentPos.x += size.x;
        }

        px::Texture atlasTexture(sizeNeededForAtlas.toUnsignedInt());

        for (const NamedTexture &texture : textures)
        {
            px::Vec2i size = texture.texture.getSize().toInt();

            for (int y = 0; y < size.y; ++y)
            {
                for (int x = 0; x < size.x; ++x)
                {
                    TextureIndex index = m_indices[texture.name];
                    px::Vec2i pos      = {x, y};
                    px::Vec2i atlasPos = index.location + pos;

                    atlasTexture.setPixel(atlasPos.toUnsignedInt(),
                                          texture.texture.getPixel(pos.toUnsignedInt()));
                }
            }
        }

        atlasTexture.saveToFile("src/atlas.png");
    }

  private:
    std::unordered_map<std::string, TextureIndex> m_indices;
};
