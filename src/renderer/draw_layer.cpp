#include "renderer.hpp"

void Renderer::draw_layer(SDL_GPUCommandBuffer *commandBuffer, Uint32 instanceCount)
{
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.0f, 0.0f, 0.0f, 0.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_drawTextures[1];
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_drawPipeline);

    SDL_GPUBuffer *buffers[5] = {m_objectBuffer.buffer(), m_textureBuffer.buffer(),
                                 m_rectangleBuffer.buffer(), m_ellipseBuffer.buffer(), m_lineBuffer.buffer()};
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, buffers, 5);
    SDL_BindGPUFragmentStorageBuffers(renderPass, 0, buffers, 5);

    float screenSize[2] = {(float)m_width, (float)m_height};
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &screenSize, sizeof(screenSize));

    SDL_GPUTextureSamplerBinding atlasBinding{};
    atlasBinding.texture = m_atlas.getGPUTexture();
    atlasBinding.sampler = m_sampler;

    SDL_BindGPUFragmentSamplers(renderPass, 0, &atlasBinding, 1);

    SDL_DrawGPUPrimitives(renderPass, 6, instanceCount, 0, 0);

    SDL_EndGPURenderPass(renderPass);
}
