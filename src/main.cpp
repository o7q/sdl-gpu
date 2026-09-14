#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <iostream>
#include <px/graphics/texture.hpp>
#include <px/math/angle.hpp>
#include <renderer/atlas.hpp>
#include <renderer/renderer.hpp>
#include <renderer/types.hpp>
#include <vector>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Renderer renderer({1920, 1080});

    std::vector<IndexedTexture> textures;

    textures.push_back({0, {"src/images/bedcat.png"}});
    textures.push_back({1, {"src/images/green shart.png"}});
    textures.push_back({2, {"src/images/sparklecat.png"}});
    textures.push_back({3, {"src/images/squisher4.png"}});
    textures.push_back({4, {"src/images/funniest boy transparent.png"}});

    renderer.atlas().uploadTextures(textures);
    renderer.atlas().getTexture().saveToFile("src/atlas.png");

    px::Angle rotation;

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;
    bool running      = true;
    while (running)
    {
        std::vector<DrawLayer> queue;
        DrawLayer a, b, c;

        b.zIndex    = 1;
        b.blendMode = DrawLayerBlendMode::Xor;
        c.zIndex    = 2;
        c.blendMode = DrawLayerBlendMode::Or;

        for (int i = 0; i < 1000; ++i)
        {
            int x = i % 20;
            int y = i / 20;
            {
                TextureDrawInfo tex;
                tex.id       = 1;
                tex.position = {x * 100.0f, y * 100.0f};
                tex.origin   = {0, 0};
                tex.scale    = {0.1f, 0.1f};
                a.queue.push_back({0, tex});
            }
            {
                TextureDrawInfo tex;
                tex.id       = 0;
                tex.position = {x * 100.0f, y * 100.0f};
                tex.origin   = {0, 0};
                tex.scale    = {0.2f, 0.2f};
                b.queue.push_back({0, tex});
            }
            {
                RectangleDrawInfo tex;
                tex.position = {x * 100.0f, y * 100.0f};
                tex.origin   = {0, 0};
                tex.size     = {30, 30};
                tex.color    = {255, 0, 0, 255};
                a.queue.push_back({1, tex});
            }
            {
                EllipseDrawInfo tex;
                tex.position = {x * 100.0f, y * 100.0f};
                tex.origin   = {0, 0};
                tex.size     = {70, 70};
                tex.color    = {255, 255, 0, 255};
                b.queue.push_back({1, tex});
            }
            // {
            //     TextureDrawInfo tex;
            //     tex.id       = 2;
            //     tex.position = {300, 300};
            //     tex.scale    = {2, 2};
            //     tex.opacity  = 0.5f;
            //     c.queue.push_back({0, tex});
            // }
        }

        queue.push_back(a);
        queue.push_back(b);
        queue.push_back(c);

        rotation = rotation + px::Degrees{0.02f};

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

        renderer.draw(queue);

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
