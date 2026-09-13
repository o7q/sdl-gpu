#include "renderer.hpp"

void Renderer::draw_layer_offscreen(SDL_GPUCommandBuffer *commandBuffer)
{
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.1f, 0.1f, 0.1f, 1.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_drawTexture;
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_spritePipeline);

    SDL_GPUTextureSamplerBinding spriteTextureBinding{};
    spriteTextureBinding.sampler = m_sampler;
    spriteTextureBinding.texture = m_atlasTexture;
    SDL_BindGPUFragmentSamplers(renderPass, 0, &spriteTextureBinding, 1);

    SDL_BindGPUVertexStorageBuffers(renderPass, 0, &m_spriteBuffer, 1);

    float screenSize[2] = {(float)m_width, (float)m_height};
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &screenSize, sizeof(screenSize));

    SDL_DrawGPUPrimitives(renderPass, 6, m_sprites.size(), 0, 0);

    SDL_EndGPURenderPass(renderPass);
}
