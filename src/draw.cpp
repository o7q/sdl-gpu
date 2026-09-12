#include "renderer.hpp"
#include <SDL3/SDL_timer.h>

void Renderer::draw()
{
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

    // RENDER
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.1f, 0.1f, 0.1f, 1.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_texture;
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_spritePipeline);

    SpriteUniform uniform;
    uniform.sprite.position = {10, 10};
    uniform.sprite.size     = {30, 30};
    uniform.sprite.angle    = SDL_GetTicks() / 1000.0f;
    uniform.screenSize      = {(float)m_width, (float)m_height};

    SDL_PushGPUVertexUniformData(commandBuffer, 0, &uniform, sizeof(uniform));
    SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass);
    //

    // BLIT
    SDL_GPUTexture *swapchainTexture;
    Uint32 width, height;
    SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, m_window, &swapchainTexture, &width, &height);

    if (!swapchainTexture)
    {
        SDL_SubmitGPUCommandBuffer(commandBuffer);
        return;
    }

    SDL_GPUColorTargetInfo colorTargetInfo2{};
    colorTargetInfo2.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    colorTargetInfo2.load_op     = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo2.store_op    = SDL_GPU_STOREOP_STORE;
    colorTargetInfo2.texture     = swapchainTexture;

    SDL_GPURenderPass *renderPass2 = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo2, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_renderPipeline);

    SDL_GPUTextureSamplerBinding textureBinding;
    textureBinding.texture = m_texture;
    textureBinding.sampler = m_sampler;
    SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);

    float ratios[2]{(float)width / (float)height, (float)m_width / (float)m_height};

    SDL_PushGPUVertexUniformData(commandBuffer, 0, &ratios, sizeof(ratios));
    SDL_DrawGPUPrimitives(renderPass2, 6, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass2);
    //

    SDL_SubmitGPUCommandBuffer(commandBuffer);
}
