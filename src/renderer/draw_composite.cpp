#include "renderer.hpp"

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
