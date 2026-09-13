#include "renderer.hpp"

void Renderer::draw_blit_to_swapchain_texture(SDL_GPUCommandBuffer *commandBuffer,
                                              SDL_GPUTexture *swapchainTexture, Uint32 width, Uint32 height)
{
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    colorTargetInfo.load_op     = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op    = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture     = swapchainTexture;

    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_renderPipeline);

    SDL_GPUTextureSamplerBinding textureBinding;
    textureBinding.texture = m_drawTexture;
    textureBinding.sampler = m_sampler;
    SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);

    float ratios[2]{(float)width / (float)height, (float)m_width / (float)m_height};
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &ratios, sizeof(ratios));

    float size[2]{(float)m_width, (float)m_height};
    SDL_PushGPUFragmentUniformData(commandBuffer, 0, &size, sizeof(size));

    SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass);
}
