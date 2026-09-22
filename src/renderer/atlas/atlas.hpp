#pragma once

#include <SDL3/SDL_gpu.h>
#include <cstdint>
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
    size_t atlasIndex = 0;
};

class GPUTextureAtlas
{
  public:
    void assignGPU(SDL_GPUDevice *device);
    void purge();
    void purgeGPUCache();

    uint64_t pushTexture(px::Texture &texture);
    void sync(bool dumpAtlasTextures = false);
    SDL_GPUTexture *getGPUTexture() { return m_atlasGPUTexture; }

    AtlasUVIndex getIndex(uint64_t id) const;

    px::Vec2u getAtlasSize() const { return m_atlasSize; }

  private:
    void create_atlases(bool dumpAtlasTextures);

    void upload_to_gpu(const std::vector<px::Texture> &atlases);

  private:
    SDL_GPUDevice *p_device           = nullptr;
    SDL_GPUTexture *m_atlasGPUTexture = nullptr;

    px::Vec2u m_atlasSize;
    px::Vec2u m_maxAtlasSize = {2048, 2048};
    std::unordered_map<uint64_t, AtlasUVIndex> m_indices;
    std::unordered_map<uint64_t, px::Texture> m_textureCache;
    uint64_t m_currentIndex = 1;
    bool m_upToDate         = true;
};
