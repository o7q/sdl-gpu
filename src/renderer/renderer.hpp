#pragma once

#include "atlas.hpp"
#include "types.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <px/graphics/texture.hpp>
#include <px/math/vec.hpp>
#include <renderer/object_buffer.hpp>
#include <vector>

inline SDL_GPUDevice *stageGPUDevice()
{
    return SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
}

class Renderer
{
  public:
    Renderer(Uint32 width, Uint32 height)
        : m_device(stageGPUDevice()), m_atlas(m_device), m_objectBuffer(m_device, m_maxObjects),
          m_textureBuffer(m_device, m_maxObjects), m_rectangleBuffer(m_device, m_maxObjects),
          m_ellipseBuffer(m_device, m_maxObjects)
    {
        m_width  = width;
        m_height = height;

        init();
    }
    ~Renderer()
    {
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_drawPipeline);
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_blitPipeline);

        SDL_ReleaseGPUTexture(m_device, m_drawTexture);
        SDL_ReleaseGPUSampler(m_device, m_sampler);

        SDL_DestroyGPUDevice(m_device);
        SDL_DestroyWindow(m_window);

        SDL_Quit();
    }

    void draw(std::vector<DrawInfo> &queue);
    GPUTextureAtlas &atlas() { return m_atlas; }

  private:
    void draw_upload_queue(std::vector<DrawInfo> &queue, SDL_GPUCommandBuffer *commandBuffer);
    void draw_to_offscreen_texture(SDL_GPUCommandBuffer *commandBuffer, Uint32 instanceCount);
    void draw_blit_to_swapchain_texture(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *swapchainTexture,
                                        Uint32 width, Uint32 height);

    void init();

    void init_draw_pipeline();
    void init_blit_pipeline();
    void init_draw_texture();

  private:
    SDL_GPUDevice *m_device = nullptr;
    SDL_Window *m_window    = nullptr;

    SDL_GPUGraphicsPipeline *m_drawPipeline = nullptr;
    SDL_GPUGraphicsPipeline *m_blitPipeline = nullptr;

    Uint32 m_width = 0, m_height = 0;
    SDL_GPUTexture *m_drawTexture = nullptr;
    SDL_GPUSampler *m_sampler     = nullptr;

    GPUTextureAtlas m_atlas;
    Uint32 m_maxObjects = 100000;
    ObjectBuffer<GPUDrawObject> m_objectBuffer;
    ObjectBuffer<GPUTextureObject> m_textureBuffer;
    ObjectBuffer<GPURectangleObject> m_rectangleBuffer;
    ObjectBuffer<GPUEllipseObject> m_ellipseBuffer;
};
