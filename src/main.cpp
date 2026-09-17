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

    Renderer renderer({512, 512});

    uint64_t bedcat      = renderer.atlas().pushTexture({"src/images/bedcat.png"});
    uint64_t greenShart  = renderer.atlas().pushTexture({"src/images/green shart.png"});
    uint64_t sparkleCat  = renderer.atlas().pushTexture({"src/images/sparklecat.png"});
    uint64_t squisher4   = renderer.atlas().pushTexture({"src/images/squisher4.png"});
    uint64_t funniestBoi = renderer.atlas().pushTexture({"src/images/funniest boy transparent.png"});
    uint64_t fuzzball    = renderer.atlas().pushTexture({"src/images/fuzzball.png"});

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;
    bool running      = true;

    float angle = 0.0f;

    while (running)
    {
        std::vector<DrawLayer> queue;
        {
            DrawLayer layer;
            layer.blendMode = DrawLayerBlendMode::Xor;

            TextureDrawInfo tex;
            tex.id       = sparkleCat;
            tex.position = {0, 0};
            tex.origin   = {0, 0};
            layer.sprites.push_back({0, tex});
            queue.push_back(layer);
        }
        {
            DrawLayer layer;
            layer.zIndex    = 1;
            layer.blendMode = DrawLayerBlendMode::Xor;

            TextureDrawInfo tex;
            tex.id       = fuzzball;
            tex.position = px::Vec2f{300, 300} + px::Vec2f{std::cos(angle), std::sin(angle)} * 50.0f;
            layer.sprites.push_back({0, tex});


            LineDrawInfo line;
            line.a = {10, 10};
            line.b = {500, 500};
            line.color = {0, 255, 0};
            line.thickness = 50;
            layer.sprites.push_back({1, line});

            queue.push_back(layer);
        }
        angle += 0.0007f;

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

        renderer.draw(queue, {0, 255, 255}, true);

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
