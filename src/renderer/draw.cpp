#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h>

void Renderer::draw(std::vector<DrawInfo> &queue)
{
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);
    draw_upload_queue(queue, commandBuffer);

    draw_to_offscreen_texture(commandBuffer, queue.size());

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
