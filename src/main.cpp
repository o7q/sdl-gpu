#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <filesystem>
#include <iostream>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <px/math/angle.hpp>
#include <px/time/time_measure.hpp>
#include <renderer/renderer.hpp>
#include <renderer/types.hpp>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Renderer renderer(px::Vec2u{512, 512});

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

    int count = 0;

    while (running)
    {
        px::TimeMeasure measure;
        measure.start();

        DrawCommand command;
        DrawLayer layer;
        for (int i = 0; i < 1; ++i)
        {
            EllipseDrawInfo ellipse;
            ellipse.position = {i * 50.0f + 200, 200};
            ellipse.size     = {300, 50};
            ellipse.color = {255, 0, 0, 255};
            ellipse.fill = true;
            ellipse.angle = px::Degrees{45};
            ellipse.outlineThickness = 3;
            layer.addSprite(ellipse, i);
        }
        command.addLayer(layer, 0, DrawLayerBlendMode::AlphaOver);
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

        renderer.draw(command, {25, 10, 40}, true);

        measure.stop();

            float framerate = 1000.0f / measure.getElapsedTime<std::milli>();
            std::cout << framerate << std::endl;
    }

    return 0;
}
