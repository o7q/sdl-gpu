#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h>
#include <px/log.hpp>
#include <renderer/types.hpp>

void Renderer::draw(std::vector<DrawLayer> &layerQueue)
{
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

    std::sort(layerQueue.begin(), layerQueue.end(),
              [](const DrawLayer &a, const DrawLayer &b) { return a.zIndex < b.zIndex; });

    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color  = {0.1f, 0.1f, 0.1f, 1.0f};
    colorTargetInfo.load_op      = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op     = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture      = m_drawTextures[0];
    SDL_GPURenderPass *clearPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_EndGPURenderPass(clearPass);

    for (DrawLayer &layer : layerQueue)
    {
        if (layer.queue.size() <= m_maxObjects)
        {
            draw_upload_queue(layer.queue, commandBuffer);
        }
        draw_layer(commandBuffer, (Uint32)layer.queue.size());
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
