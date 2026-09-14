#include "renderer.hpp"
#include <algorithm>
#include <px/variant_bp.hpp>
#include <renderer/atlas.hpp>
#include <renderer/types.hpp>

void Renderer::draw_upload_queue(std::vector<DrawInfo> &queue, SDL_GPUCommandBuffer *commandBuffer)
{
    std::sort(queue.begin(), queue.end(),
              [](const DrawInfo &a, const DrawInfo &b) { return a.zIndex < b.zIndex; });

    std::vector<GPUDrawObject> objects;
    objects.reserve(queue.size());
    std::vector<GPUTextureObject> textures;
    std::vector<GPURectangleObject> rectangles;
    std::vector<GPUEllipseObject> ellipses;

    for (const DrawInfo &info : queue)
    {
        std::visit(
            px::variant::overloaded{[this, &objects, &textures](const TextureDrawInfo &info_)
                                    {
                                        AtlasUVIndex index    = m_atlas.getIndex(info_.id);
                                        px::Vec2f atlasSize   = m_atlas.getTexture().getSize().toFloat();
                                        px::Vec2f textureSize = index.size * atlasSize;
                                        px::Vec2f textureSizeScaled = textureSize * info_.scale;

                                        GPUDrawObject object;
                                        object.type    = 0;
                                        object.index   = (int)textures.size();
                                        object.filter  = info_.filter;
                                        object.opacity = info_.opacity;
                                        objects.push_back(object);

                                        GPUTextureObject texture;
                                        texture.atlasPositionNormal[0] = index.position.x;
                                        texture.atlasPositionNormal[1] = index.position.y;
                                        texture.atlasSizeNormal[0]     = index.size.x;
                                        texture.atlasSizeNormal[1]     = index.size.y;
                                        texture.position[0]            = info_.position.x;
                                        texture.position[1]            = info_.position.y;
                                        texture.size[0]                = textureSizeScaled.x;
                                        texture.size[1]                = textureSizeScaled.y;
                                        texture.origin[0] =
                                            info_.origin.x.value_or(textureSizeScaled.x / 2.0f);
                                        texture.origin[1] =
                                            info_.origin.y.value_or(textureSizeScaled.y / 2.0f);
                                        texture.angle              = info_.angle.radians();
                                        texture.cropTopLeft[0]     = info_.cropTopLeft.x / textureSize.x;
                                        texture.cropTopLeft[1]     = info_.cropTopLeft.y / textureSize.y;
                                        texture.cropBottomRight[0] = info_.cropBottomRight.x / textureSize.x;
                                        texture.cropBottomRight[1] = info_.cropBottomRight.y / textureSize.y;
                                        textures.push_back(texture);
                                    },
                                    [&objects, &rectangles](const RectangleDrawInfo &info_)
                                    {
                                        GPUDrawObject object;
                                        object.type    = 1;
                                        object.index   = (int)rectangles.size();
                                        object.filter  = info_.filter;
                                        object.opacity = info_.opacity;
                                        objects.push_back(object);

                                        GPURectangleObject rectangle;
                                        rectangle.position[0] = info_.position.x;
                                        rectangle.position[1] = info_.position.y;
                                        rectangle.size[0]     = info_.size.x;
                                        rectangle.size[1]     = info_.size.y;
                                        rectangle.origin[0]   = info_.origin.x.value_or(info_.size.x / 2.0f);
                                        rectangle.origin[1]   = info_.origin.y.value_or(info_.size.y / 2.0f);
                                        rectangle.angle       = info_.angle.radians();
                                        px::Vec4f color       = info_.color.normalize();
                                        rectangle.color[0]    = color.x;
                                        rectangle.color[1]    = color.y;
                                        rectangle.color[2]    = color.z;
                                        rectangle.color[3]    = color.w;
                                        rectangles.push_back(rectangle);
                                    },
                                    [&objects, &ellipses](const EllipseDrawInfo &info_)
                                    {
                                        GPUDrawObject object;
                                        object.type    = 2;
                                        object.index   = (int)ellipses.size();
                                        object.filter  = info_.filter;
                                        object.opacity = info_.opacity;
                                        objects.push_back(object);

                                        GPUEllipseObject ellipse;
                                        ellipse.position[0] = info_.position.x;
                                        ellipse.position[1] = info_.position.y;
                                        ellipse.size[0]     = info_.size.x;
                                        ellipse.size[1]     = info_.size.y;
                                        ellipse.origin[0]   = info_.origin.x.value_or(info_.size.x / 2.0f);
                                        ellipse.origin[1]   = info_.origin.y.value_or(info_.size.y / 2.0f);
                                        ellipse.angle       = info_.angle.radians();
                                        px::Vec4f color     = info_.color.normalize();
                                        ellipse.color[0]    = color.x;
                                        ellipse.color[1]    = color.y;
                                        ellipse.color[2]    = color.z;
                                        ellipse.color[3]    = color.w;
                                        ellipses.push_back(ellipse);
                                    }},
            info.info);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    m_objectBuffer.upload(copyPass, objects);
    m_textureBuffer.upload(copyPass, textures);
    m_rectangleBuffer.upload(copyPass, rectangles);
    m_ellipseBuffer.upload(copyPass, ellipses);

    SDL_EndGPUCopyPass(copyPass);
}