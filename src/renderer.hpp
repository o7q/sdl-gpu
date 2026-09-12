#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <iostream>

struct Vec2
{
    float x = 0.0f, y = 0.0f;
};

struct Sprite
{
    Vec2 position;
    Vec2 size;
    float angle = 0.0f;
    float _pad1[3];
    float r = 1.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

struct SpriteUniform
{
    Sprite sprite;
    Vec2 screenSize;
};

class Renderer
{
  public:
    Renderer(Uint32 width, Uint32 height)
    {
        m_width  = width;
        m_height = height;

        SDL_Init(SDL_INIT_VIDEO);
        m_window = SDL_CreateWindow("Renderer", 1280, 720, SDL_WINDOW_RESIZABLE);
        if (!m_window)
        {
            std::cout << "Failed to create window!" << std::endl;
            return;
        }

        m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
        if (!m_device)
        {
            std::cout << "Failed to create device!" << std::endl;
            return;
        }

        if (!SDL_ClaimWindowForGPUDevice(m_device, m_window))
        {
            std::cout << "Error claiming window!" << std::endl;
            return;
        }

        SDL_SetGPUSwapchainParameters(m_device, m_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                      SDL_GPU_PRESENTMODE_IMMEDIATE);

        init_sprite_pipeline();
        init_render_pipeline();

        SDL_GPUTextureCreateInfo textureInfo{};
        textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        textureInfo.width                = width;
        textureInfo.height               = height;
        textureInfo.num_levels           = 1;
        textureInfo.layer_count_or_depth = 1;
        textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
        m_texture                        = SDL_CreateGPUTexture(m_device, &textureInfo);

        SDL_GPUSamplerCreateInfo samplerInfo{};
        samplerInfo.min_filter     = SDL_GPU_FILTER_NEAREST;
        samplerInfo.mag_filter     = SDL_GPU_FILTER_NEAREST;
        samplerInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        m_sampler                  = SDL_CreateGPUSampler(m_device, &samplerInfo);

        Uint64 startTicks = SDL_GetTicks();
        int frameCount    = 0;
        bool running      = true;
        while (running)
        {
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

            draw();

            Uint64 currentTicks = SDL_GetTicks();
            frameCount++;
            if (currentTicks - startTicks >= 1000)
            {
                std::cout << frameCount << std::endl;
                frameCount = 0;
                startTicks = currentTicks;
            }
        }

        SDL_DestroyGPUDevice(m_device);
        SDL_DestroyWindow(m_window);

        SDL_Quit();
    }

  private:
    void draw();

    void init_sprite_pipeline();
    void init_render_pipeline();

  private:
    SDL_Window *m_window;
    SDL_GPUDevice *m_device;

    Uint32 m_width, m_height;
    SDL_GPUTexture *m_texture = nullptr;
    SDL_GPUSampler *m_sampler = nullptr;

    SDL_GPUGraphicsPipeline *m_spritePipeline;
    SDL_GPUGraphicsPipeline *m_renderPipeline;
};
