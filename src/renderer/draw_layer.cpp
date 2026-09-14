#include "renderer.hpp"
#include <SDL3/SDL_gpu.h>

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

void Renderer::draw_composite(SDL_GPUCommandBuffer *commandBuffer, DrawLayerBlendMode blendMode)
{
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.0f, 0.0f, 0.0f, 0.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_compositeTexture;
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_compositePipeline);

    // samplers
    SDL_GPUTextureSamplerBinding layer0Binding{};
    layer0Binding.texture = m_drawTextures[0];
    layer0Binding.sampler = m_sampler;
    SDL_GPUTextureSamplerBinding layer1Binding{};
    layer1Binding.texture                    = m_drawTextures[1];
    layer1Binding.sampler                    = m_sampler;
    SDL_GPUTextureSamplerBinding bindings[2] = {layer0Binding, layer1Binding};
    SDL_BindGPUFragmentSamplers(renderPass, 0, bindings, 2);
    //

    int blendModeIndex = (int)blendMode;
    SDL_PushGPUFragmentUniformData(commandBuffer, 0, &blendModeIndex, sizeof(blendModeIndex));

    SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);
    SDL_EndGPURenderPass(renderPass);

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    SDL_GPUTextureLocation location{};
    location.texture = m_compositeTexture;

    SDL_GPUTextureLocation dest{};
    dest.texture = m_drawTextures[0];

    SDL_CopyGPUTextureToTexture(copyPass, &location, &dest, m_width, m_height, 1, true);
    SDL_EndGPUCopyPass(copyPass);
}
