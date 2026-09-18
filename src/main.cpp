#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <filesystem>
#include <iostream>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <px/math/angle.hpp>
#include <renderer/renderer.hpp>
#include <renderer/types.hpp>
#include <vector>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Renderer renderer({1024, 512});

    uint64_t bedcat      = renderer.uploadStaticTexture({"src/images/bedcat.png"});
    uint64_t greenShart  = renderer.uploadStaticTexture({"src/images/green shart.png"});
    uint64_t sparkleCat  = renderer.uploadStaticTexture({"src/images/sparklecat.png"});
    uint64_t squisher4   = renderer.uploadStaticTexture({"src/images/squisher4.png"});
    uint64_t funniestBoi = renderer.uploadStaticTexture({"src/images/funniest boy transparent.png"});
    uint64_t fuzzball    = renderer.uploadStaticTexture({"src/images/fuzzball.png"});

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;
    bool running      = true;

    float angle = 0.0f;

    while (running)
    {
        DrawCommand command;

        for (int i = 0; i < 20; ++i)
        {
            DrawLayer layer;
    
            TextureDrawInfo tex;
            tex.id       = sparkleCat;
            tex.position = {i * 100.0f, 0};
            tex.origin   = {0, 0};
            layer.addSprite(tex, 0);
    
            command.addLayer(layer, i, DrawLayerBlendMode::AlphaOver);
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

        renderer.draw(command, {255, 0, 0}, true);

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
