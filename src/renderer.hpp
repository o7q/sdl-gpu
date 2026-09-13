#pragma once

#include "variant_bp.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <variant>
#include <vector>

struct Vec2
{
    float x = 0.0f, y = 0.0f;
};

struct RectangleDrawInfo
{
    Vec2 position;
    Vec2 size;
    float angle;
};

struct CircleDrawInfo
{
    Vec2 position;
    float radius;
};

using DrawInfoType = std::variant<RectangleDrawInfo, CircleDrawInfo>;

struct DrawInfo
{
    int index = 0;
    DrawInfoType info;
};

struct GPURectangleObject
{
    Vec2 position;
    Vec2 size;
    float angle;
    float _pad[3];
};

struct GPUCircleObject
{
    Vec2 position;
    float radius;
    float _pad[3];
};

struct GPUDrawObject
{
    // 0: rectangle
    // 1: circle
    // used to determine which object buffer to read from
    int objectType;
    // index into the object buffer
    int index;
    int _pad[2];
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

        init_draw_pipeline();
        init_blit_pipeline();

        init_draw_texture();

        init_object_buffers();

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

            draw(queue);

            Uint64 currentTicks = SDL_GetTicks();
            frameCount++;
            if (currentTicks - startTicks >= 1000)
            {
                std::cout << frameCount << std::endl;
                frameCount = 0;
                startTicks = currentTicks;
            }
        }

        free_object_buffer(&m_objectBuffer);
        free_object_buffer(&m_rectangleBuffer);
        free_object_buffer(&m_circleBuffer);

        SDL_ReleaseGPUGraphicsPipeline(m_device, m_drawPipeline);
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_blitPipeline);

        SDL_ReleaseGPUTexture(m_device, m_drawTexture);
        SDL_ReleaseGPUSampler(m_device, m_sampler);

        SDL_DestroyGPUDevice(m_device);
        SDL_DestroyWindow(m_window);

        SDL_Quit();
    }

  private:
    void draw_upload_queue(std::vector<DrawInfo> &queue, SDL_GPUCommandBuffer *commandBuffer)
    {
        std::sort(queue.begin(), queue.end(),
                  [](const DrawInfo &a, const DrawInfo &b) { return a.index < b.index; });

        std::vector<GPUDrawObject> objects;
        std::vector<GPURectangleObject> rectObjects;
        std::vector<GPUCircleObject> circleObjects;

        for (DrawInfo &info : queue)
        {
            std::visit(overloaded{[&objects, &rectObjects](const RectangleDrawInfo &info_)
                                  {
                                      GPUDrawObject object;
                                      object.objectType = 0;
                                      object.index      = (int)rectObjects.size();

                                      GPURectangleObject rectObject;
                                      rectObject.position = info_.position;
                                      rectObject.size     = info_.size;
                                      rectObject.angle    = info_.angle;

                                      objects.push_back(object);
                                      rectObjects.push_back(rectObject);
                                  },
                                  [&objects, &circleObjects](const CircleDrawInfo &info_)
                                  {
                                      GPUDrawObject object;
                                      object.objectType = 1;
                                      object.index      = (int)circleObjects.size();

                                      GPUCircleObject circleObject;
                                      circleObject.position = info_.position;
                                      circleObject.radius   = info_.radius;

                                      objects.push_back(object);
                                      circleObjects.push_back(circleObject);
                                  }},
                       info.info);
        }

        SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

        upload_object_buffer(copyPass, &m_objectBuffer, objects.data(),
                             objects.size() * sizeof(GPUDrawObject));
        upload_object_buffer(copyPass, &m_rectangleBuffer, rectObjects.data(),
                             rectObjects.size() * sizeof(GPURectangleObject));
        upload_object_buffer(copyPass, &m_circleBuffer, circleObjects.data(),
                             circleObjects.size() * sizeof(GPUCircleObject));

        SDL_EndGPUCopyPass(copyPass);
    }

    void draw(std::vector<DrawInfo> &queue);

    void draw_to_offscreen_texture(SDL_GPUCommandBuffer *commandBuffer, Uint32 instanceCount);

    void draw_blit_to_swapchain_texture(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *swapchainTexture,
                                        Uint32 width, Uint32 height);

    void init_draw_texture();

    void init_draw_pipeline();
    void init_blit_pipeline();

    struct GPUObjectBuffer
    {
        SDL_GPUBuffer *buffer           = nullptr;
        SDL_GPUTransferBuffer *transfer = nullptr;
    };

    void upload_object_buffer(SDL_GPUCopyPass *copyPass, GPUObjectBuffer *buffer, void *data, size_t bytes)
    {
        void *mapping = SDL_MapGPUTransferBuffer(m_device, buffer->transfer, true);
        std::memcpy(mapping, data, bytes);
        SDL_UnmapGPUTransferBuffer(m_device, buffer->transfer);

        SDL_GPUTransferBufferLocation location{};
        location.transfer_buffer = buffer->transfer;
        location.offset          = 0;

        SDL_GPUBufferRegion region{};
        region.buffer = buffer->buffer;
        region.size   = (Uint32)bytes;
        region.offset = 0;

        SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
    }

    void init_object_buffer(GPUObjectBuffer *buffer, Uint32 bytes)
    {
        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.size  = bytes;
        bufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
        buffer->buffer   = SDL_CreateGPUBuffer(m_device, &bufferInfo);

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.size  = bytes;
        transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        buffer->transfer   = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);
    }

    void init_object_buffers()
    {
        init_object_buffer(&m_objectBuffer, sizeof(GPUDrawObject) * m_maxBufferSize);
        init_object_buffer(&m_rectangleBuffer, sizeof(GPURectangleObject) * m_maxBufferSize);
        init_object_buffer(&m_circleBuffer, sizeof(GPUCircleObject) * m_maxBufferSize);
    }

    void free_object_buffer(GPUObjectBuffer *buffer)
    {
        SDL_ReleaseGPUBuffer(m_device, buffer->buffer);
        SDL_ReleaseGPUTransferBuffer(m_device, buffer->transfer);
    }

  private:
    SDL_Window *m_window    = nullptr;
    SDL_GPUDevice *m_device = nullptr;

    SDL_GPUGraphicsPipeline *m_drawPipeline = nullptr;
    SDL_GPUGraphicsPipeline *m_blitPipeline = nullptr;

    Uint32 m_width = 0, m_height = 0;
    SDL_GPUTexture *m_drawTexture = nullptr;
    SDL_GPUSampler *m_sampler     = nullptr;

    Uint32 m_maxBufferSize = 10000;
    GPUObjectBuffer m_objectBuffer;
    GPUObjectBuffer m_rectangleBuffer;
    GPUObjectBuffer m_circleBuffer;
};
