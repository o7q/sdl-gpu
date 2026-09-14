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
#include <vector>

class Renderer
{
  private:
    struct TextureObject
    {
        float atlasPositionNormal[2];
        float atlasSizeNormal[2];
        float position[2];
        float size[2];
        float origin[2];
        float angle;
        float _pad0;
        float cropTopLeft[2];
        float cropBottomRight[2];
    };

    struct RectangleObject
    {
        float position[2];
        float size[2];
        float origin[2];
        float angle;
        float _pad0;
        float color[4];
    };

    struct EllipseObject
    {
        float position[2];
        float size[2];
        float origin[2];
        float angle;
        float _pad0;
        float color[4];
    };

    struct DrawObject
    {
        int type;
        float _pad0;
        TextureObject texture;
        float _pad1[2];
        RectangleObject rectangle;
        EllipseObject ellipse;
        float opacity;
        int filter;
        float _pad2[2];
    };

  public:
    Renderer(Uint32 width, Uint32 height) : m_atlas(m_device)
    {
        m_width  = width;
        m_height = height;

        init();
    }
    ~Renderer()
    {
        SDL_ReleaseGPUBuffer(m_device, m_objectBuffer);
        SDL_ReleaseGPUTransferBuffer(m_device, m_objectTransferBuffer);

        SDL_ReleaseGPUGraphicsPipeline(m_device, m_drawPipeline);
        SDL_ReleaseGPUGraphicsPipeline(m_device, m_blitPipeline);

        SDL_ReleaseGPUTexture(m_device, m_drawTexture);
        SDL_ReleaseGPUSampler(m_device, m_sampler);

        SDL_DestroyGPUDevice(m_device);
        SDL_DestroyWindow(m_window);

        SDL_Quit();
    }

    void draw(std::vector<DrawInfo> &queue);
    TextureAtlas &atlas() { return m_atlas; }

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
    TextureAtlas m_atlas;

    SDL_Window *m_window    = nullptr;
    SDL_GPUDevice *m_device = nullptr;

    SDL_GPUGraphicsPipeline *m_drawPipeline = nullptr;
    SDL_GPUGraphicsPipeline *m_blitPipeline = nullptr;

    Uint32 m_width = 0, m_height = 0;
    SDL_GPUTexture *m_drawTexture = nullptr;
    SDL_GPUSampler *m_sampler     = nullptr;

    Uint32 m_maxObjects                           = 100000;
    SDL_GPUBuffer *m_objectBuffer                 = nullptr;
    SDL_GPUTransferBuffer *m_objectTransferBuffer = nullptr;
};
