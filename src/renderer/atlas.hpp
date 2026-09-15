#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <cstdint>
#include <cstring>
#include <px/graphics/texture.hpp>
#include <px/log.hpp>
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
    size_t atlasIndex;
};

class GPUTextureAtlas
{
  public:
    void init(SDL_GPUDevice *device) { p_device = device; }
    void destroyGPUCache() { SDL_ReleaseGPUTexture(p_device, m_atlasGPUTexture); }

    uint64_t pushTexture(px::Texture &texture)
    {
        m_upToDate                     = false;
        m_textureCache[m_currentIndex] = texture;
        return m_currentIndex++;
    }
    uint64_t pushTexture(px::Texture &&texture) { return pushTexture(texture); }
    void sync(bool dumpAtlasTextures = false)
    {
        if (m_upToDate)
        {
            return;
        }

        destroyGPUCache();
        create_atlases(dumpAtlasTextures);

        m_upToDate = true;
    }
    SDL_GPUTexture *getGPUTexture() { return m_atlasGPUTexture; }

    AtlasUVIndex getIndex(uint64_t id) const;

    px::Vec2u getAtlasSize() const { return m_atlasSize; }

  private:
    void create_atlases(bool dumpAtlasTextures);

    void upload_to_gpu(const std::vector<px::Texture> &atlases);

  private:
    SDL_GPUDevice *p_device;
    SDL_GPUTexture *m_atlasGPUTexture = nullptr;

    px::Vec2u m_atlasSize = {1024, 1024};
    std::unordered_map<uint64_t, AtlasUVIndex> m_indices;
    std::unordered_map<uint64_t, px::Texture> m_textureCache;
    uint64_t m_currentIndex = 1;
    bool m_upToDate         = true;
};
