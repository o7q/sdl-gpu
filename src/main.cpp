#include "atlas.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <iostream>
#include <px/graphics/texture.hpp>
#include <string>
#include <vector>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    std::vector<px::Texture> textures;

    for (const auto &entry : std::filesystem::recursive_directory_iterator("src/images"))
    {
        if (entry.path().extension().string() != ".png")
        {
            continue;
        }
        textures.push_back({entry.path()});
    }

    TextureAtlas atlas = createTextureAtlas(textures, 4096);

    for (TextureReference ref : atlas.references)
    {
        std::cout << ref.location.x << " " << ref.location.y << std::endl;
        std::cout << ref.size.x << " " << ref.size.y << std::endl;

        std::cout << std::endl;
    }

    atlas.atlasTexture.saveToFile("src/atlas.png");

    return 0;
}
