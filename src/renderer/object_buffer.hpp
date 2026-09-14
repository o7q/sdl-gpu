#pragma once

#include <SDL3/SDL_gpu.h>
#include <cstring>
#include <vector>

template <typename T> class ObjectBuffer
{
  public:
    void init(SDL_GPUDevice *device, Uint32 maxObjects)
    {
        Uint32 bytes = maxObjects * sizeof(T);

        SDL_GPUBufferCreateInfo bufferInfo{};
        bufferInfo.size  = bytes;
        bufferInfo.usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ;
        m_buffer         = SDL_CreateGPUBuffer(device, &bufferInfo);

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.size  = bytes;
        transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        m_transfer         = SDL_CreateGPUTransferBuffer(device, &transferInfo);

        p_device = device;
    }
    void destroy()
    {
        SDL_ReleaseGPUBuffer(p_device, m_buffer);
        SDL_ReleaseGPUTransferBuffer(p_device, m_transfer);
    }

    void upload(SDL_GPUCopyPass *copyPass, const std::vector<T> &objects)
    {
        Uint32 bytes = objects.size() * sizeof(T);

        void *mapping = SDL_MapGPUTransferBuffer(p_device, m_transfer, true);
        std::memcpy(mapping, objects.data(), bytes);
        SDL_UnmapGPUTransferBuffer(p_device, m_transfer);

        SDL_GPUTransferBufferLocation location{};
        location.transfer_buffer = m_transfer;
        location.offset          = 0;

        SDL_GPUBufferRegion region{};
        region.buffer = m_buffer;
        region.size   = (Uint32)bytes;
        region.offset = 0;

        SDL_UploadToGPUBuffer(copyPass, &location, &region, true);
    }

    SDL_GPUTransferBuffer *transfer() { return m_transfer; }
    SDL_GPUBuffer *buffer() { return m_buffer; }

  private:
    SDL_GPUDevice *p_device;

    SDL_GPUBuffer *m_buffer           = nullptr;
    SDL_GPUTransferBuffer *m_transfer = nullptr;
};
