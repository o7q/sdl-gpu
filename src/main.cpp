#include <SDL3/SDL_init.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <iostream>
#include <px/graphics/texture.hpp>
#include <renderer/renderer.hpp>
#include <renderer/types.hpp>
#include <vector>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Renderer renderer({1920, 1080});

    std::vector<NamedTexture> textures;

    textures.push_back({"bedcat", {"src/images/bedcat.png"}});
    textures.push_back({"green", {"src/images/green shart.png"}});
    textures.push_back({"sparkle", {"src/images/sparklecat.png"}});
    textures.push_back({"squisher", {"src/images/squisher4.png"}});

    renderer.atlas().uploadTextures(textures);
    renderer.atlas().getTexture().saveToFile("src/atlas.png");

    float rotation = 0.0f;

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;
    bool running      = true;
    while (running)
    {
        std::vector<DrawInfo> queue;

        for (int i = 0; i < 10000; ++i)
        {
            float x = i % 100;
            float y = i / 100;

            // EllipseDrawInfo tex;
            // tex.position = {x * 100.0f, y * 100.0f};
            // tex.size     = {100, 100};
            // tex.angle    = rotation;
            // queue.push_back({0, tex});
            TextureDrawInfo tex;
            tex.name     = "sparkle";
            tex.position = {x * 50.0f, y * 50.0f};
            tex.size     = {50, 50};
            tex.angle    = rotation;
            queue.push_back({0, tex});
        }
        rotation += 0.001f;

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
