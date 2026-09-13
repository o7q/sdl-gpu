#include "renderer/renderer.hpp"
#include "renderer/types.hpp"
#include <iostream>

int main()
{
    Renderer renderer(256, 128);

    float rotation = 0.0f;

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;
    bool running      = true;
    while (running)
    {
        std::vector<DrawInfo> queue;

        CircleDrawInfo circle;
        circle.position = {50, 50};
        circle.radius   = 30;
        queue.push_back({1, circle});

        RectangleDrawInfo rect;
        rect.position = {60, 30};
        rect.size     = {60, 20};
        rect.angle    = rotation;
        queue.push_back({0, rect});
        rotation += 0.0001f;
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
