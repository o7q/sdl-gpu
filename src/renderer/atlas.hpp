#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <algorithm>
#include <cstring>
#include <px/graphics/texture.hpp>
#include <px/math/vec.hpp>
#include <unordered_map>
#include <vector>

struct IndexedTexture
{
    uint64_t id;
    px::Texture texture;
};

struct AtlasUVIndex
{
    px::Vec2f position;
    px::Vec2f size;
};

class TextureAtlas
{
  public:
    TextureAtlas(SDL_GPUDevice *&device) : r_device(device) {}
    ~TextureAtlas() { SDL_ReleaseGPUTexture(r_device, m_atlasGPUTexture); }

    void uploadTextures(std::vector<IndexedTexture> &textures)
    {
        create_atlas(textures);
        upload_to_gpu();
    }
    const px::Texture &getTexture() const { return m_atlasTexture; }
    SDL_GPUTexture *getGPUTexture() { return m_atlasGPUTexture; }

    AtlasUVIndex getIndex(uint64_t id) const
    {
        auto it = m_indices.find(id);
        return it != m_indices.end() ? it->second : AtlasUVIndex{};
    }

  private:
    void create_atlas(std::vector<IndexedTexture> &textures)
    {
        std::sort(textures.begin(), textures.end(), [](const IndexedTexture &a, const IndexedTexture &b)
                  { return a.texture.getSize().y > b.texture.getSize().y; });

        std::unordered_map<uint64_t, px::Vec2i> positionsTemp;
        positionsTemp.reserve(textures.size());

        px::Vec2i sizeNeededForAtlas;
        px::Vec2i currentPos;
        int tallestTexture = 0;
        for (const IndexedTexture &texture : textures)
        {
            px::Vec2i size = texture.texture.getSize().toInt();

            if (currentPos.x + size.x > m_maxWidth)
            {
                currentPos.x = 0;
                currentPos.y += tallestTexture;
                tallestTexture = 0;
            }

            if (size.y > tallestTexture)
            {
                tallestTexture = size.y;
            }

            positionsTemp[texture.id] = currentPos;
            sizeNeededForAtlas.x      = std::max(currentPos.x + size.x, sizeNeededForAtlas.x);
            sizeNeededForAtlas.y      = std::max(currentPos.y + size.y, sizeNeededForAtlas.y);

            currentPos.x += size.x;
        }

        m_atlasTexture = px::Texture(sizeNeededForAtlas.toUnsignedInt());

        for (const IndexedTexture &texture : textures)
        {
            px::Vec2i size = texture.texture.getSize().toInt();

            px::Vec2i topLeft     = positionsTemp[texture.id];
            m_indices[texture.id] = {topLeft.toFloat() / sizeNeededForAtlas.toFloat(),
                                     size.toFloat() / sizeNeededForAtlas.toFloat()};

            for (int y = 0; y < size.y; ++y)
            {
                for (int x = 0; x < size.x; ++x)
                {
                    px::Vec2i pos      = {x, y};
                    px::Vec2i atlasPos = topLeft + pos;

                    m_atlasTexture.setPixel(atlasPos.toUnsignedInt(),
                                            texture.texture.getPixel(pos.toUnsignedInt()));
                }
            }
        }
    }

    void upload_to_gpu()
    {
        px::Vec2u size = m_atlasTexture.getSize();

        SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(r_device);
        SDL_GPUCopyPass *copyPass           = SDL_BeginGPUCopyPass(commandBuffer);

        SDL_GPUTextureCreateInfo textureInfo{};
        textureInfo.type                 = SDL_GPU_TEXTURETYPE_2D;
        textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        textureInfo.width                = (Uint32)size.x;
        textureInfo.height               = (Uint32)size.y;
        textureInfo.num_levels           = 1;
        textureInfo.layer_count_or_depth = 1;
        textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER;
        m_atlasGPUTexture                = SDL_CreateGPUTexture(r_device, &textureInfo);

        size_t bytes = (size_t)size.x * (size_t)size.y * 4;

        SDL_GPUTransferBufferCreateInfo transferInfo{};
        transferInfo.size                     = (Uint32)bytes;
        transferInfo.usage                    = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(r_device, &transferInfo);

        Uint8 *data = (Uint8 *)SDL_MapGPUTransferBuffer(r_device, transferBuffer, false);
        std::memcpy(data, reinterpret_cast<Uint8 *>(m_atlasTexture.getPixels().data()), bytes);

        SDL_GPUTextureTransferInfo source{};
        source.transfer_buffer = transferBuffer;
        source.offset          = 0;

        SDL_GPUTextureRegion region{};
        region.texture = m_atlasGPUTexture;
        region.w       = (Uint32)size.x;
        region.h       = (Uint32)size.y;
        region.d       = 1;

        SDL_UploadToGPUTexture(copyPass, &source, &region, false);

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }

  private:
    SDL_GPUDevice *&r_device;
    SDL_GPUTexture *m_atlasGPUTexture = nullptr;
    px::Texture m_atlasTexture;

    int m_maxWidth = 256;
    std::unordered_map<uint64_t, AtlasUVIndex> m_indices;
};
