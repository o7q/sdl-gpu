#include "atlas.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <px/graphics/texture.hpp>
#include <vector>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    std::vector<NamedTexture> textures;

    for (const auto &entry : std::filesystem::recursive_directory_iterator("src/images"))
    {
        if (entry.path().extension().string() != ".png")
        {
            continue;
        }
        textures.push_back({entry.path(), px::Texture(entry.path())});
    }

    TextureAtlas atlas(textures, 256);

    return 0;
}
