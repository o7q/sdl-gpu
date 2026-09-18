#pragma once

#include "atlas/atlas.hpp"
#include "gpu_types.hpp"
#include "renderer/object_buffer.hpp"
#include "types.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <px/math/vec.hpp>

class GPUDevice
{
  public:
    GPUDevice() { m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr); }
    SDL_GPUDevice *get() { return m_device; }

  private:
    SDL_GPUDevice *m_device = nullptr;
};

class Renderer
{
  public:
    Renderer(Uint32 width, Uint32 height, Uint32 maxObjects = 100000) : m_maxObjects(maxObjects)
    {
        m_width  = width;
        m_height = height;

        init();
    }
    ~Renderer() { destroy(); }

    void draw(const DrawCommand &command, px::Color clearColor = {0, 0, 0, 255},
              bool dumpSyncedAtlases = false);
    uint64_t uploadStaticTexture(px::Texture &texture) { return m_atlas.pushTexture(texture); }
    uint64_t uploadStaticTexture(px::Texture &&texture) { return uploadStaticTexture(texture); }

  private:
    void draw_upload_objects(const DrawCommand &command, SDL_GPUCommandBuffer *commandBuffer);
    void draw_layer(SDL_GPUCommandBuffer *commandBuffer, Uint32 startObject, Uint32 objectCount);
    void draw_composite(SDL_GPUCommandBuffer *commandBuffer, DrawLayerBlendMode blendMode);
    void draw_blit_to_swapchain_texture(SDL_GPUCommandBuffer *commandBuffer, SDL_GPUTexture *swapchainTexture,
                                        Uint32 width, Uint32 height);

    void init();
    void init_window_and_device();
    void init_atlases();
    void init_buffers();
    void init_draw_pipeline();
    void init_composite_pipeline();
    void init_blit_pipeline();
    void init_textures_and_samplers();

    void destroy();
    void destroy_window_and_device();
    void destroy_atlases();
    void destroy_buffers();
    void destroy_draw_pipeline();
    void destroy_composite_pipeline();
    void destroy_blit_pipeline();
    void destroy_textures_and_samplers();

  private:
    SDL_GPUDevice *m_device = nullptr;
    SDL_Window *m_window    = nullptr;

    SDL_GPUGraphicsPipeline *m_drawPipeline      = nullptr;
    SDL_GPUGraphicsPipeline *m_compositePipeline = nullptr;
    SDL_GPUGraphicsPipeline *m_blitPipeline      = nullptr;

    Uint32 m_width = 0, m_height = 0;
    SDL_GPUTexture *m_drawTextures[2]  = {nullptr, nullptr};
    SDL_GPUTexture *m_compositeTexture = nullptr;
    SDL_GPUSampler *m_sampler          = nullptr;

    GPUTextureAtlas m_atlas;
    const Uint32 m_maxObjects = 1;
    ObjectBuffer<GPUDrawObject> m_objectBuffer;
    ObjectBuffer<GPUTextureObject> m_textureBuffer;
    ObjectBuffer<GPUBasicShapeObject> m_shapeBuffer;
};
