#include "renderer.hpp"

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <iostream>

void Renderer::init()
{
    SDL_Init(SDL_INIT_VIDEO);

    init_window_and_device();
    init_atlases();
    init_buffers();
    init_draw_pipeline();
    init_composite_pipeline();
    init_blit_pipeline();
    init_textures_and_samplers();

    SDL_SetGPUSwapchainParameters(m_device, m_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  SDL_GPU_PRESENTMODE_IMMEDIATE);
}

void Renderer::init_window_and_device()
{
    m_window = SDL_CreateWindow("Renderer", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (!m_window)
    {
        std::cout << "Failed to create window!" << std::endl;
        return;
    }

    m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, nullptr);
    if (!m_device)
    {
        std::cout << "Error creating device!" << std::endl;
        return;
    }

    if (!SDL_ClaimWindowForGPUDevice(m_device, m_window))
    {
        std::cout << "Error claiming window!" << std::endl;
        return;
    }
}

void Renderer::init_atlases() { m_atlas.init(m_device); }

void Renderer::init_buffers()
{
    m_objectBuffer.init(m_device, m_maxObjects);
    m_textureBuffer.init(m_device, m_maxObjects);
    m_shapeBuffer.init(m_device, m_maxObjects);
}

void Renderer::init_textures_and_samplers()
{
    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width                = m_width;
    textureInfo.height               = m_height;
    textureInfo.num_levels           = 1;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    m_drawTextures[0]                = SDL_CreateGPUTexture(m_device, &textureInfo);
    m_drawTextures[1]                = SDL_CreateGPUTexture(m_device, &textureInfo);
    m_compositeTexture               = SDL_CreateGPUTexture(m_device, &textureInfo);

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter     = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter     = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    m_sampler                  = SDL_CreateGPUSampler(m_device, &samplerInfo);
}
