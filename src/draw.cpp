#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_timer.h>
#include <cstring>

void Renderer::draw()
{
    // for (Sprite &sprite : m_sprites)
    // {
    //     sprite.position.x += 0.01f;
    //     sprite.angle += 0.001f;
    // }

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

    // COPY
    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    void *data = SDL_MapGPUTransferBuffer(m_device, m_spriteTransferBuffer, true);
    std::memcpy(data, m_sprites.data(), m_sprites.size() * sizeof(Sprite));
    SDL_UnmapGPUTransferBuffer(m_device, m_spriteTransferBuffer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = m_spriteTransferBuffer;
    location.offset          = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = m_spriteBuffer;
    region.size   = sizeof(Sprite) * m_sprites.size();
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);
    //

    // RENDER
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.1f, 0.1f, 0.1f, 1.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_drawTexture;
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_spritePipeline);

    SpriteUniform uniform;
    uniform.screenSize = {(float)m_width, (float)m_height};

    SDL_GPUTextureSamplerBinding spriteTextureBinding{};
    spriteTextureBinding.sampler = m_sampler;
    spriteTextureBinding.texture = m_atlasTexture;

    SDL_BindGPUFragmentSamplers(renderPass, 0, &spriteTextureBinding, 1);
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &m_spriteBuffer, 1);
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &uniform, sizeof(uniform));
    SDL_DrawGPUPrimitives(renderPass, 6, m_sprites.size(), 0, 0);

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
    SDL_BindGPUGraphicsPipeline(renderPass2, m_renderPipeline);

    SDL_GPUTextureSamplerBinding textureBinding;
    textureBinding.texture = m_drawTexture;
    textureBinding.sampler = m_sampler;
    SDL_BindGPUFragmentSamplers(renderPass2, 0, &textureBinding, 1);

    float ratios[2]{(float)width / (float)height, (float)m_width / (float)m_height};
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &ratios, sizeof(ratios));

    float size[2]{(float)m_width, (float)m_height};
    SDL_PushGPUFragmentUniformData(commandBuffer, 0, &size, sizeof(size));

    SDL_DrawGPUPrimitives(renderPass2, 6, 1, 0, 0);

    SDL_EndGPURenderPass(renderPass2);
    //

    SDL_SubmitGPUCommandBuffer(commandBuffer);
}
