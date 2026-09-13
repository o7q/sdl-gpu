#include "renderer.hpp"

void Renderer::upload_object_buffer(SDL_GPUCopyPass *copyPass, ObjectBuffer *buffer, void *data, size_t bytes)
{
    void *mapping = SDL_MapGPUTransferBuffer(m_device, buffer->transfer, true);
    std::memcpy(mapping, data, bytes);
    SDL_UnmapGPUTransferBuffer(m_device, buffer->transfer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = buffer->transfer;
    location.offset          = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = buffer->buffer;
    region.size   = (Uint32)bytes;
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
}

void Renderer::init_object_buffer(ObjectBuffer *buffer, Uint32 bytes)
{
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size  = bytes;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
    buffer->buffer   = SDL_CreateGPUBuffer(m_device, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size  = bytes;
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    buffer->transfer   = SDL_CreateGPUTransferBuffer(m_device, &transferInfo);
}

void Renderer::free_object_buffer(ObjectBuffer *buffer)
{
    SDL_ReleaseGPUBuffer(m_device, buffer->buffer);
    SDL_ReleaseGPUTransferBuffer(m_device, buffer->transfer);
}