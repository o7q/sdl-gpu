#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

inline uint32_t hash(uint32_t value, uint32_t seed)
{
    value ^= seed;
    value ^= value >> 16;
    value *= 0x85ebca6b;
    value ^= value >> 13;
    value *= 0xc2b2ae35;
    value ^= value >> 16;
    return value;
}

inline uint32_t randomInt(int32_t min, int32_t max)
{
    if (min >= max)
    {
        return min;
    }

    uint32_t minU  = static_cast<uint32_t>(min);
    uint32_t maxU  = static_cast<uint32_t>(max);
    uint32_t range = maxU - minU;

    static std::minstd_rand gen(std::random_device{}());
    return static_cast<int32_t>(static_cast<uint32_t>(gen()) % range + minU);
}

struct Vec2
{
    float x = 0.0f, y = 0.0f;
};

struct Sprite
{
    Vec2 position;
    Vec2 size;
    float angle = 0.0f;
    float _pad[3];
    float r = 1.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

struct SpriteUniform
{
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

        init_texture();
        init_sprites_buffer();

        SDL_Surface *surface   = SDL_LoadPNG("src/sparklecat.png");
        SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
        SDL_DestroySurface(surface);

        if (!converted)
        {
            std::cout << "Error creating texture!" << std::endl;
            return;
        }

        SDL_GPUTextureCreateInfo textureInfo{};
        textureInfo.type                 = SDL_GPU_TEXTURETYPE_2D;
        textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        textureInfo.width                = converted->w;
        textureInfo.height               = converted->h;
        textureInfo.layer_count_or_depth = 1;
        textureInfo.num_levels           = 1;
        m_atlasTexture                   = SDL_CreateGPUTexture(m_device, &textureInfo);

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.usage    = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferInfo.size     = converted->w * converted->h * 4;
        m_atlasTransferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);

        void *data = SDL_MapGPUTransferBuffer(m_device, m_atlasTransferBuffer, false);
        std::memcpy(data, converted->pixels, converted->w * converted->h * 4);
        SDL_UnmapGPUTransferBuffer(m_device, m_atlasTransferBuffer);

        SDL_GPUTextureTransferInfo textureTransferInfo{};
        textureTransferInfo.transfer_buffer = m_atlasTransferBuffer;
        textureTransferInfo.offset          = 0;

        SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);
        SDL_GPUCopyPass *copyPass           = SDL_BeginGPUCopyPass(commandBuffer);

        SDL_GPUTextureRegion region{};
        region.texture = m_atlasTexture;
        region.w       = converted->w;
        region.h       = converted->h;
        region.d       = 1;

        SDL_UploadToGPUTexture(copyPass, &textureTransferInfo, &region, false);
        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(commandBuffer);

        for (int i = 0; i < 10; ++i)
        {
            Sprite sprite;
            sprite.position = {(float)randomInt(0, m_width), (float)randomInt(0, m_height)};
            float rand = (float)randomInt(5, 200);
            sprite.size     = {rand, rand};
            sprite.angle    = (float)randomInt(5, 20);
            sprite.r        = randomInt(0, 255) / 255.0f;
            sprite.g        = randomInt(0, 255) / 255.0f;
            sprite.b        = randomInt(0, 255) / 255.0f;
            m_sprites.push_back(sprite);
        }

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

            // m_sprites.clear();

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

        SDL_ReleaseGPUTransferBuffer(m_device, m_spriteTransferBuffer);
        SDL_ReleaseGPUBuffer(m_device, m_spriteBuffer);

        SDL_ReleaseGPUTransferBuffer(m_device, m_atlasTransferBuffer);
        SDL_ReleaseGPUTexture(m_device, m_atlasTexture);

        SDL_ReleaseGPUGraphicsPipeline(m_device, m_spritePipeline);
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_renderPipeline);

        SDL_DestroyGPUDevice(m_device);
        SDL_DestroyWindow(m_window);

        SDL_Quit();
    }

  private:
    void draw();

    void init_sprite_pipeline();
    void init_render_pipeline();
    void init_texture()
    {
        SDL_GPUTextureCreateInfo textureInfo{};
        textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        textureInfo.width                = m_width;
        textureInfo.height               = m_height;
        textureInfo.num_levels           = 1;
        textureInfo.layer_count_or_depth = 1;
        textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
        m_drawTexture                    = SDL_CreateGPUTexture(m_device, &textureInfo);

        SDL_GPUSamplerCreateInfo samplerInfo{};
        samplerInfo.min_filter     = SDL_GPU_FILTER_NEAREST;
        samplerInfo.mag_filter     = SDL_GPU_FILTER_NEAREST;
        samplerInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
        samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
        m_sampler                  = SDL_CreateGPUSampler(m_device, &samplerInfo);
    }

    void init_sprites_buffer()
    {
        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.size  = sizeof(Sprite) * m_maxSprites;
        bufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
        m_spriteBuffer   = SDL_CreateGPUBuffer(m_device, &bufferInfo);

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.size      = sizeof(Sprite) * m_maxSprites;
        transferInfo.usage     = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        m_spriteTransferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);
    }

  private:
    SDL_Window *m_window;
    SDL_GPUDevice *m_device;

    Uint32 m_width, m_height;
    SDL_GPUTexture *m_drawTexture = nullptr;
    SDL_GPUSampler *m_sampler     = nullptr;

    std::vector<Sprite> m_sprites;
    Uint32 m_maxSprites                           = 100000;
    SDL_GPUBuffer *m_spriteBuffer                 = nullptr;
    SDL_GPUTransferBuffer *m_spriteTransferBuffer = nullptr;

    SDL_GPUTexture *m_atlasTexture               = nullptr;
    SDL_GPUTransferBuffer *m_atlasTransferBuffer = nullptr;

    SDL_GPUGraphicsPipeline *m_spritePipeline;
    SDL_GPUGraphicsPipeline *m_renderPipeline;
};
