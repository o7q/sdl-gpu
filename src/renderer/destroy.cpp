#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>

void Renderer::destroy()
{
    destroy_textures_and_samplers();
    destroy_blit_pipeline();
    destroy_composite_pipeline();
    destroy_draw_pipeline();
    destroy_buffers();
    destroy_atlases();
    destroy_window_and_device();

    SDL_Quit();
}
void Renderer::destroy_window_and_device()
{
    SDL_DestroyGPUDevice(m_device);
    SDL_DestroyWindow(m_window);
}
void Renderer::destroy_atlases() { m_atlas.purgeGPUCache(); }
void Renderer::destroy_buffers()
{
    m_objectBuffer.destroy();
    m_textureBuffer.destroy();
    m_shapeBuffer.destroy();
}
void Renderer::destroy_draw_pipeline() { SDL_ReleaseGPUGraphicsPipeline(m_device, m_drawPipeline); }
void Renderer::destroy_composite_pipeline() { SDL_ReleaseGPUGraphicsPipeline(m_device, m_compositePipeline); }
void Renderer::destroy_blit_pipeline() { SDL_ReleaseGPUGraphicsPipeline(m_device, m_blitPipeline); }
void Renderer::destroy_textures_and_samplers()
{
    SDL_ReleaseGPUSampler(m_device, m_sampler);
    SDL_ReleaseGPUTexture(m_device, m_drawTextures[2]);
    SDL_ReleaseGPUTexture(m_device, m_drawTextures[1]);
    SDL_ReleaseGPUTexture(m_device, m_drawTextures[0]);
}
