#pragma once

#include "types.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <vector>

class Renderer
{
  private:
    struct RectangleObject
    {
        Vec2 position;
        Vec2 size;
        float angle;
        float _pad[3];
    };

    struct CircleObject
    {
        Vec2 position;
        float radius;
        float _pad[3];
    };

    struct DrawObject
    {
        // 0: rectangle
        // 1: circle
        // used to determine which object buffer to read from
        int objectType;
        // index into the object buffer
        int index;
        int _pad[2];
    };

    struct ObjectBuffer
    {
        SDL_GPUBuffer *buffer           = nullptr;
        SDL_GPUTransferBuffer *transfer = nullptr;
    };

  public:
    Renderer(Uint32 width, Uint32 height)
    {
        m_width  = width;
        m_height = height;

        init();
    }
    ~Renderer()
    {
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

    void draw(std::vector<DrawInfo> &queue);

  private:
    void draw_upload_queue(std::vector<DrawInfo> &queue, SDL_GPUCommandBuffer *commandBuffer);
    void draw_to_offscreen_texture(SDL_GPUCommandBuffer *commandBuffer, Uint32 instanceCount);
    void draw_blit_to_swapchain_texture(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *swapchainTexture,
                                        Uint32 width, Uint32 height);

    void init();
    void init_draw_pipeline();
    void init_blit_pipeline();
    void init_draw_texture();

    void upload_object_buffer(SDL_GPUCopyPass *copyPass, ObjectBuffer *buffer, void *data, size_t bytes)
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

    void init_object_buffer(ObjectBuffer *buffer, Uint32 bytes)
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
        init_object_buffer(&m_objectBuffer, sizeof(DrawObject) * m_maxBufferSize);
        init_object_buffer(&m_rectangleBuffer, sizeof(RectangleObject) * m_maxBufferSize);
        init_object_buffer(&m_circleBuffer, sizeof(CircleObject) * m_maxBufferSize);
    }

    void free_object_buffer(ObjectBuffer *buffer)
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
    ObjectBuffer m_objectBuffer;
    ObjectBuffer m_rectangleBuffer;
    ObjectBuffer m_circleBuffer;
};
