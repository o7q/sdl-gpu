#include "renderer.hpp"

void Renderer::draw_to_offscreen_texture(SDL_GPUCommandBuffer *commandBuffer, Uint32 instanceCount)
{
    SDL_GPUColorTargetInfo colorTargetInfo{};
    colorTargetInfo.clear_color   = {0.1f, 0.1f, 0.1f, 1.0f};
    colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture       = m_drawTexture;
    SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_BindGPUGraphicsPipeline(renderPass, m_drawPipeline);

    SDL_GPUBuffer *buffers[3]{m_objectBuffer.buffer, m_rectangleBuffer.buffer, m_circleBuffer.buffer};
    SDL_BindGPUVertexStorageBuffers(renderPass, 0, buffers, 3);

    float screenSize[2] = {(float)m_width, (float)m_height};
    SDL_PushGPUVertexUniformData(commandBuffer, 0, &screenSize, sizeof(screenSize));

    SDL_DrawGPUPrimitives(renderPass, 6, instanceCount, 0, 0);

    SDL_EndGPURenderPass(renderPass);
}
