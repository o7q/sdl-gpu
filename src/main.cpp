#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <iostream>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <px/math/angle.hpp>
#include <renderer/atlas.hpp>
#include <renderer/renderer.hpp>
#include <renderer/types.hpp>
#include <vector>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Renderer renderer({64, 64});

    std::vector<IndexedTexture> textures;

    textures.push_back({0, {"src/images/bedcat.png"}});
    textures.push_back({1, {"src/images/green shart.png"}});
    textures.push_back({2, {"src/images/sparklecat.png"}});
    textures.push_back({3, {"src/images/squisher4.png"}});
    textures.push_back({4, {"src/images/funniest boy transparent.png"}});

    renderer.atlas().uploadTextures(textures);
    renderer.atlas().getTexture().saveToFile("src/atlas.png");

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;
    bool running      = true;
    while (running)
    {
        std::vector<DrawLayer> queue;
        DrawLayer layer;

        // RectangleDrawInfo rect;
        // rect.size = {4, 4};
        // layer.sprites.push_back({0, rect});

        px::Vec2f offset = {0, 0};

        LineDrawInfo line;
        line.a         = px::Vec2f{0, 0} + offset;
        line.b         = px::Vec2f{20, 10} + offset;
        line.thickness = 1;
        layer.sprites.push_back({0, line});

        queue.push_back(layer);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }

        renderer.draw(queue, px::Color{255, 0, 0, 100});

        Uint64 currentTicks = SDL_GetTicks();
        frameCount++;
        if (currentTicks - startTicks >= 1000)
        {
            std::cout << frameCount << std::endl;
            frameCount = 0;
            startTicks = currentTicks;
        }
    }

    return 0;
}
