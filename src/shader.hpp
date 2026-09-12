#pragma once

#include <SDL3/SDL_gpu.h>
#include <string>

inline SDL_GPUShader *loadShader(SDL_GPUDevice *device, const std::string &path, SDL_GPUShaderStage stage,
                                  SDL_GPUShaderFormat format, Uint32 samplerCount, Uint32 storageBufferCount,
                                  Uint32 storageTextureCount, Uint32 uniformCount)
{
    size_t codeSize;
    void *code = SDL_LoadFile(path.data(), &codeSize);
    
    SDL_GPUShaderCreateInfo info{};
    info.code                 = (Uint8 *)code;
    info.code_size            = codeSize;
    info.entrypoint           = "main";
    info.format               = format;
    info.stage                = stage;
    info.num_samplers         = samplerCount;
    info.num_storage_buffers  = storageBufferCount;
    info.num_storage_textures = storageTextureCount;
    info.num_uniform_buffers  = uniformCount;
    SDL_GPUShader *shader     = SDL_CreateGPUShader(device, &info);
    SDL_free(code);
    return shader;
}
