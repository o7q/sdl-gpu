#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h>
#include <px/log.hpp>
#include <renderer/types.hpp>

void Renderer::draw(std::vector<DrawLayer> &layerQueue, px::Color clearColor, bool dumpSyncedAtlases)
{
    std::vector<DrawLayer> queueCopy = layerQueue;

    m_atlas.sync(dumpSyncedAtlases);

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

    std::sort(queueCopy.begin(), queueCopy.end(),
              [](const DrawLayer &a, const DrawLayer &b) { return a.zIndex < b.zIndex; });

    SDL_GPUColorTargetInfo colorTargetInfo{};
    px::Vec4f clearColorNormal   = clearColor.normalize();
    colorTargetInfo.clear_color  = {clearColorNormal.x, clearColorNormal.y, clearColorNormal.z,
                                    clearColorNormal.w};
    colorTargetInfo.load_op      = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op     = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture      = m_drawTextures[0];
    SDL_GPURenderPass *clearPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_EndGPURenderPass(clearPass);

    for (DrawLayer &layer : queueCopy)
    {
        if (layer.sprites.size() >= m_maxObjects)
        {
            continue;
        }
        draw_upload_queue(layer.sprites, commandBuffer);
        draw_layer(commandBuffer, (Uint32)layer.sprites.size());
        draw_composite(commandBuffer, layer.blendMode);
    }

    SDL_GPUTexture *swapchainTexture;
    Uint32 width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, m_window, &swapchainTexture, &width, &height);

    if (!swapchainTexture)
    {
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return;
    }

    draw_blit_to_swapchain_texture(commandBuffer, swapchainTexture, width, height);

    SDL_SubmitGPUCommandBuffer(commandBuffer);
}
