#include "atlas.hpp"
#include <SDL3/SDL_gpu.h>
#include <cstdint>
#include <iostream>
#include <px/graphics/color.hpp>
#include <px/graphics/texture.hpp>
#include <utility>
#include <vector>

AtlasUVIndex GPUTextureAtlas::getIndex(uint64_t id) const
{
    auto it = m_indices.find(id);
    if (it == m_indices.end())
    {
        px::log::error("px::GPUTextureAtlas", "Error getting UV index! Invalid ID: ", id);
        return {};
    }
    return it->second;
}

void GPUTextureAtlas::create_atlases(bool dumpAtlasTextures)
{
    using TextureRef = std::pair<uint64_t, px::Texture *>;
    std::vector<TextureRef> textureRefs;
    textureRefs.reserve(m_textureCache.size());

    for (auto &[id, texture] : m_textureCache)
    {
        textureRefs.push_back({id, &texture});
    }

    std::sort(textureRefs.begin(), textureRefs.end(), [](const TextureRef &a, const TextureRef &b)
              { return a.second->getSize().y > b.second->getSize().y; });

    size_t atlasIndex = 0;
    std::vector<px::Texture> atlases;

    px::Vec2i currentPos;
    int tallestTexture = 0;

    for (const TextureRef &textureRef : textureRefs)
    {
        px::Vec2i size = textureRef.second->getSize().toInt();

        if (size.x > (int)m_atlasSize.x || size.y > (int)m_atlasSize.y)
        {
            std::cout << textureRef.first << ": Texture too big! Skipping" << std::endl;
            continue;
        }

        if (currentPos.x + size.x > (int)m_atlasSize.x)
        {
            currentPos.x = 0;
            currentPos.y += tallestTexture;
            tallestTexture = 0;
        }

        if (size.y > tallestTexture)
        {
            tallestTexture = size.y;
        }

        if (currentPos.y + size.y >= (int)m_atlasSize.y)
        {
            currentPos = {};
            atlasIndex++;
        }

        if (atlasIndex >= atlases.size())
        {
            atlases.push_back(px::Texture(m_atlasSize));
        }

        AtlasUVIndex index;
        index.position              = currentPos.toFloat() / m_atlasSize.toFloat();
        index.size                  = size.toFloat() / m_atlasSize.toFloat();
        index.atlasIndex            = atlasIndex;
        m_indices[textureRef.first] = index;

        for (int y = 0; y < size.y; ++y)
        {
            for (int x = 0; x < size.x; ++x)
            {
                px::Vec2i pos      = {x, y};
                px::Vec2i atlasPos = currentPos + pos;

                px::Vec4f color_normal = textureRef.second->getPixel(pos.toUnsignedInt()).normalize();

                color_normal.x *= color_normal.w;
                color_normal.y *= color_normal.w;
                color_normal.z *= color_normal.w;

                atlases[atlasIndex].setPixel(atlasPos.toUnsignedInt(), color_normal, px::BlendMode::Replace);
            }
        }

        currentPos.x += size.x;
    }

    if (dumpAtlasTextures)
    {
        for (size_t i = 0; i < atlases.size(); ++i)
        {
            atlases[i].saveToFile("src/atlas" + std::to_string(i) + ".png");
        }
    }

    upload_to_gpu(atlases);
}

void GPUTextureAtlas::upload_to_gpu(const std::vector<px::Texture> &atlases)
{
    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(p_device);
    SDL_GPUCopyPass *copyPass           = SDL_BeginGPUCopyPass(commandBuffer);

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.type                 = SDL_GPU_TEXTURETYPE_2D_ARRAY;
    textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width                = (Uint32)m_atlasSize.x;
    textureInfo.height               = (Uint32)m_atlasSize.y;
    textureInfo.num_levels           = 1;
    textureInfo.layer_count_or_depth = (Uint32)atlases.size();
    textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    m_atlasGPUTexture                = SDL_CreateGPUTexture(p_device, &textureInfo);

    size_t bytesPerTexture = (size_t)m_atlasSize.x * (size_t)m_atlasSize.y * 4;
    size_t bytesTotal      = bytesPerTexture * atlases.size();

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size                     = (Uint32)bytesTotal;
    transferInfo.usage                    = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(p_device, &transferInfo);

    Uint8 *data   = (Uint8 *)SDL_MapGPUTransferBuffer(p_device, transferBuffer, false);
    size_t offset = 0;
    for (const px::Texture &texture : atlases)
    {
        std::memcpy(data + offset, reinterpret_cast<const Uint8 *>(texture.getPixels().data()),
                    bytesPerTexture);
        offset += bytesPerTexture;
    }
    SDL_UnmapGPUTransferBuffer(p_device, transferBuffer);

    offset = 0;
    for (size_t i = 0; i < atlases.size(); ++i)
    {
        SDL_GPUTextureTransferInfo source{};
        source.transfer_buffer = transferBuffer;
        source.offset          = (Uint32)offset;

        SDL_GPUTextureRegion region{};
        region.texture = m_atlasGPUTexture;
        region.w       = (Uint32)m_atlasSize.x;
        region.h       = (Uint32)m_atlasSize.y;
        region.d       = 1;
        region.layer   = (Uint32)i;

        SDL_UploadToGPUTexture(copyPass, &source, &region, false);
        offset += bytesPerTexture;
    }

    SDL_EndGPUCopyPass(copyPass);
    SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(commandBuffer);
    SDL_WaitForGPUFences(p_device, true, &fence, 1);
    SDL_ReleaseGPUFence(p_device, fence);

    SDL_ReleaseGPUTransferBuffer(p_device, transferBuffer);
}
