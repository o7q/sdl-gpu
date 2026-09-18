#pragma once

#include <SDL3/SDL_gpu.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

inline SDL_GPUShader *loadShader(SDL_GPUDevice *device, std::vector<uint8_t> &code, SDL_GPUShaderStage stage,
                                 SDL_GPUShaderFormat format, Uint32 samplerCount, Uint32 storageBufferCount,
                                 Uint32 storageTextureCount, Uint32 uniformCount)
{
    SDL_GPUShaderCreateInfo info{};
    info.code                 = (Uint8 *)code.data();
    info.code_size            = code.size();
    info.entrypoint           = "main";
    info.format               = format;
    info.stage                = stage;
    info.num_samplers         = samplerCount;
    info.num_storage_buffers  = storageBufferCount;
    info.num_storage_textures = storageTextureCount;
    info.num_uniform_buffers  = uniformCount;
    SDL_GPUShader *shader     = SDL_CreateGPUShader(device, &info);
    return shader;
}

inline SDL_GPUShader *loadShader(SDL_GPUDevice *device, const std::string &path, SDL_GPUShaderStage stage,
                                 SDL_GPUShaderFormat format, Uint32 samplerCount, Uint32 storageBufferCount,
                                 Uint32 storageTextureCount, Uint32 uniformCount)
{
    size_t codeSize;
    void *codeRaw = SDL_LoadFile(path.data(), &codeSize);

    std::vector<uint8_t> code;
    code.resize(codeSize);

    std::string str = "";

    for (size_t i = 0; i < codeSize; ++i)
    {
        code[i] = ((uint8_t *)codeRaw)[i];
        str += std::to_string(code[i]) + ',';
    }

    // std::ofstream out("src/" + std::filesystem::path(path).filename().string());

    // out << str;
    // out.close();

    SDL_GPUShader *shader = loadShader(device, code, stage, format, samplerCount, storageBufferCount,
                                       storageTextureCount, uniformCount);
    SDL_free(codeRaw);
    return shader;
}
