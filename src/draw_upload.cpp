#include "renderer.hpp"

void Renderer::draw_upload_sprites(SDL_GPUCommandBuffer *commandBuffer)
{
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
}
