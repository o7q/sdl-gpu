#include "renderer.hpp"
#include "variant_bp.hpp"
#include <algorithm>
#include <renderer/atlas.hpp>
#include <renderer/types.hpp>

void Renderer::draw_upload_queue(std::vector<DrawInfo> &queue, SDL_GPUCommandBuffer *commandBuffer)
{
    std::sort(queue.begin(), queue.end(),
              [](const DrawInfo &a, const DrawInfo &b) { return a.index < b.index; });

    std::vector<DrawObject> objects;
    objects.reserve(queue.size());

    for (const DrawInfo &info : queue)
    {
        std::visit(overloaded{[this, &objects](const TextureDrawInfo &info_)
                              {
                                  AtlasUVIndex index = m_atlas.getIndex(info_.name);

                                  DrawObject object;
                                  object.type             = 0;
                                  object.texture.uvPos    = index.location;
                                  object.texture.uvSize   = index.size;
                                  object.texture.position = info_.position;
                                  object.texture.size     = info_.size;
                                  object.texture.angle    = info_.angle;
                                  objects.push_back(object);
                              },
                              [&objects](const RectangleDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.type               = 1;
                                  object.rectangle.position = info_.position;
                                  object.rectangle.size     = info_.size;
                                  object.rectangle.angle    = info_.angle;
                                  objects.push_back(object);
                              },
                              [&objects](const EllipseDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.type             = 2;
                                  object.ellipse.position = info_.position;
                                  object.ellipse.size     = info_.size;
                                  object.ellipse.angle    = info_.angle;
                                  objects.push_back(object);
                              }},
                   info.info);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    size_t bytes = objects.size() * sizeof(DrawObject);

    void *mapping = SDL_MapGPUTransferBuffer(m_device, m_objectTransferBuffer, true);
    std::memcpy(mapping, objects.data(), bytes);
    SDL_UnmapGPUTransferBuffer(m_device, m_objectTransferBuffer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = m_objectTransferBuffer;
    location.offset          = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = m_objectBuffer;
    region.size   = (Uint32)bytes;
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);
}