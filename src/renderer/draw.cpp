#include "renderer.hpp"

#include <px/graphics/color.hpp>
#include <px/log.hpp>
#include <renderer/types.hpp>

void Renderer::draw(const DrawCommand &command, px::Color clearColor, bool dumpSyncedAtlases)
{
    m_atlas.sync(dumpSyncedAtlases);

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);

    SDL_GPUColorTargetInfo colorTargetInfo{};
    px::Vec4f clearColorNormal   = clearColor.normalize();
    colorTargetInfo.clear_color  = {clearColorNormal.x, clearColorNormal.y, clearColorNormal.z,
                                    clearColorNormal.w};
    colorTargetInfo.load_op      = SDL_GPU_LOADOP_CLEAR;
    colorTargetInfo.store_op     = SDL_GPU_STOREOP_STORE;
    colorTargetInfo.texture      = m_drawTextures[0];
    SDL_GPURenderPass *clearPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
    SDL_EndGPURenderPass(clearPass);

    for (const DrawLayerData &layer : command.getLayers())
    {
        const std::multiset<DrawInfo> &objects = layer.layer.getObjects();
        if (objects.size() > m_maxObjects)
        {
            continue;
        }
        draw_upload_objects(objects, commandBuffer);
        draw_layer(commandBuffer, (Uint32)objects.size());
        draw_composite(commandBuffer, layer.blendMode);
    }

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
