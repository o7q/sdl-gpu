#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h>
#include <px/log.hpp>

void Renderer::draw(std::vector<DrawInfo> &queue)
{
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

    if (queue.size() <= m_maxObjects)
    {
        draw_upload_queue(queue, commandBuffer);
    }

    draw_to_offscreen_texture(commandBuffer, (Uint32)queue.size());

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
