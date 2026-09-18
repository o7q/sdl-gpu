#include "atlas.hpp"

#include <px/log.hpp>

void GPUTextureAtlas::init(SDL_GPUDevice *device) { p_device = device; }
void GPUTextureAtlas::purge()
{
    purgeGPUCache();
    m_indices.clear();
    m_textureCache.clear();
    m_currentIndex = 1;
    m_upToDate     = true;
}
void GPUTextureAtlas::purgeGPUCache() { SDL_ReleaseGPUTexture(p_device, m_atlasGPUTexture); }

uint64_t GPUTextureAtlas::pushTexture(px::Texture &texture)
{
    m_upToDate                     = false;
    m_textureCache[m_currentIndex] = texture;
    return m_currentIndex++;
}

AtlasUVIndex GPUTextureAtlas::getIndex(uint64_t id) const
{
    auto it = m_indices.find(id);
    if (it == m_indices.end())
    {
        px::log::error("px::GPUTextureAtlas::getIndex()", "Error getting UV index! Invalid ID: ", id);
        return {};
    }
    return it->second;
}
