#include "renderer.hpp"

void Renderer::draw_layer(SDL_GPUCommandBuffer *commandBuffer, Uint32 startObject, Uint32 objectCount)
{
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.0f, 0.0f, 0.0f, 0.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_drawTextures[1];
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_drawPipeline);

    SDL_GPUBuffer *buffers[3] = {m_objectBuffer.buffer(), m_textureBuffer.buffer(), m_shapeBuffer.buffer()};
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, buffers, 3);
    SDL_BindGPUFragmentStorageBuffers(renderPass, 0, buffers, 3);

    float screenSize[2] = {(float)m_width, (float)m_height};
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &screenSize, sizeof(screenSize));

    SDL_GPUTextureSamplerBinding atlasBinding{};
    atlasBinding.texture = m_atlas.getGPUTexture();
    atlasBinding.sampler = m_sampler;

    SDL_BindGPUFragmentSamplers(renderPass, 0, &atlasBinding, 1);

    SDL_DrawGPUPrimitives(renderPass, 6, objectCount, 0, startObject);

    SDL_EndGPURenderPass(renderPass);
}
