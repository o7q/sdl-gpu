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
                                  AtlasUVIndex index          = m_atlas.getIndex(info_.id);
                                  px::Vec2f atlasSize         = m_atlas.getTexture().getSize().toFloat();
                                  px::Vec2f textureSize       = index.size * atlasSize;
                                  px::Vec2f textureSizeScaled = textureSize * info_.scale;

                                  DrawObject object;
                                  object.type                           = 0;
                                  object.texture.atlasPositionNormal[0] = index.position.x;
                                  object.texture.atlasPositionNormal[1] = index.position.y;
                                  object.texture.atlasSizeNormal[0]     = index.size.x;
                                  object.texture.atlasSizeNormal[1]     = index.size.y;
                                  object.texture.position[0]            = info_.position.x;
                                  object.texture.position[1]            = info_.position.y;
                                  object.texture.size[0]                = textureSizeScaled.x;
                                  object.texture.size[1]                = textureSizeScaled.y;
                                  object.texture.origin[0] =
                                      info_.origin.x.value_or(textureSizeScaled.x / 2.0f);
                                  object.texture.origin[1] =
                                      info_.origin.y.value_or(textureSizeScaled.y / 2.0f);
                                  object.texture.angle              = info_.angle.radians();
                                  object.texture.cropTopLeft[0]     = info_.cropTopLeft.x / textureSize.x;
                                  object.texture.cropTopLeft[1]     = info_.cropTopLeft.y / textureSize.y;
                                  object.texture.cropBottomRight[0] = info_.cropBottomRight.x / textureSize.x;
                                  object.texture.cropBottomRight[1] = info_.cropBottomRight.y / textureSize.y;
                                  object.opacity                    = info_.opacity;
                                  object.filter                     = info_.filter;
                                  objects.push_back(object);
                              },
                              [&objects](const RectangleDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.type                  = 1;
                                  object.rectangle.position[0] = info_.position.x;
                                  object.rectangle.position[1] = info_.position.y;
                                  object.rectangle.size[0]     = info_.size.x;
                                  object.rectangle.size[1]     = info_.size.y;
                                  object.rectangle.origin[0]   = info_.origin.x.value_or(info_.size.x / 2.0f);
                                  object.rectangle.origin[1]   = info_.origin.y.value_or(info_.size.y / 2.0f);
                                  object.rectangle.angle       = info_.angle.radians();
                                  px::Vec4f color              = info_.color.normalize();
                                  object.rectangle.color[0]    = color.x;
                                  object.rectangle.color[1]    = color.y;
                                  object.rectangle.color[2]    = color.z;
                                  object.rectangle.color[3]    = color.w;
                                  object.opacity               = info_.opacity;
                                  object.filter                = info_.filter;
                                  objects.push_back(object);
                              },
                              [&objects](const EllipseDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.type                = 2;
                                  object.ellipse.position[0] = info_.position.x;
                                  object.ellipse.position[1] = info_.position.y;
                                  object.ellipse.size[0]     = info_.size.x;
                                  object.ellipse.size[1]     = info_.size.y;
                                  object.ellipse.origin[0]   = info_.origin.x.value_or(info_.size.x / 2.0f);
                                  object.ellipse.origin[1]   = info_.origin.y.value_or(info_.size.y / 2.0f);
                                  object.ellipse.angle       = info_.angle.radians();
                                  px::Vec4f color            = info_.color.normalize();
                                  object.rectangle.color[0]  = color.x;
                                  object.rectangle.color[1]  = color.y;
                                  object.rectangle.color[2]  = color.z;
                                  object.rectangle.color[3]  = color.w;
                                  object.opacity             = info_.opacity;
                                  object.filter              = info_.filter;
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