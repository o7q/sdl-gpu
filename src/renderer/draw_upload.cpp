#include "renderer.hpp"

#include "renderer/gpu_types.hpp"
#include "renderer/types.hpp"
#include <px/graphics/texture.hpp>
#include <px/variant_bp.hpp>

void Renderer::draw_upload_objects(const std::multiset<DrawInfo> &objects,
                                   SDL_GPUCommandBuffer *commandBuffer)
{
    std::vector<GPUDrawObject> gpuObjects;
    gpuObjects.reserve(objects.size());
    std::vector<GPUTextureObject> gpuTextures;
    std::vector<GPUBasicShapeObject> gpuShapes;

    for (const DrawInfo &object : objects)
    {
        std::visit(
            px::variant::overloaded{
                [this, &gpuObjects, &gpuTextures](const TextureDrawInfo &info_)
                {
                    AtlasUVIndex index = m_atlas.getIndex(info_.id);

                    px::Vec2f textureSize       = index.size * m_atlas.getAtlasSize().toFloat();
                    px::Vec2f textureSizeScaled = textureSize * info_.scale;

                    GPUDrawObject object;
                    object.type    = 0;
                    object.index   = (int)gpuTextures.size();
                    object.filter  = info_.filter;
                    object.opacity = info_.opacity;
                    gpuObjects.push_back(object);

                    GPUTextureObject texture;
                    texture.atlasPositionNormal[0] = index.position.x;
                    texture.atlasPositionNormal[1] = index.position.y;
                    texture.atlasSizeNormal[0]     = index.size.x;
                    texture.atlasSizeNormal[1]     = index.size.y;
                    texture.atlasIndex             = (int)index.atlasIndex;
                    texture.position[0]            = info_.position.x;
                    texture.position[1]            = info_.position.y;
                    texture.size[0]                = textureSizeScaled.x;
                    texture.size[1]                = textureSizeScaled.y;
                    texture.origin[0]      = info_.origin.x.value_or(std::floor(textureSizeScaled.x / 2.0f));
                    texture.origin[1]      = info_.origin.y.value_or(std::floor(textureSizeScaled.y / 2.0f));
                    texture.angle          = info_.angle.radians();
                    texture.cropTopLeft[0] = info_.cropTopLeft.x / textureSize.x;
                    texture.cropTopLeft[1] = info_.cropTopLeft.y / textureSize.y;
                    texture.cropBottomRight[0] = info_.cropBottomRight.x / textureSize.x;
                    texture.cropBottomRight[1] = info_.cropBottomRight.y / textureSize.y;
                    gpuTextures.push_back(texture);
                },
                [&gpuObjects, &gpuShapes](const RectangleDrawInfo &info_)
                {
                    GPUDrawObject object;
                    object.type    = 1;
                    object.index   = (int)gpuShapes.size();
                    object.filter  = info_.filter;
                    object.opacity = info_.opacity;
                    gpuObjects.push_back(object);

                    GPUBasicShapeObject rectangle;
                    rectangle.position[0] = info_.position.x;
                    rectangle.position[1] = info_.position.y;
                    rectangle.size[0]     = info_.size.x;
                    rectangle.size[1]     = info_.size.y;
                    rectangle.origin[0]   = info_.origin.x.value_or(std::floor(info_.size.x / 2.0f));
                    rectangle.origin[1]   = info_.origin.y.value_or(std::floor(info_.size.y / 2.0f));
                    rectangle.angle       = info_.angle.radians();
                    px::Vec4f color       = info_.color.normalize();
                    rectangle.color[0]    = color.x;
                    rectangle.color[1]    = color.y;
                    rectangle.color[2]    = color.z;
                    rectangle.color[3]    = color.w;
                    gpuShapes.push_back(rectangle);
                },
                [&gpuObjects, &gpuShapes](const EllipseDrawInfo &info_)
                {
                    GPUDrawObject object;
                    object.type    = 2;
                    object.index   = (int)gpuShapes.size();
                    object.filter  = info_.filter;
                    object.opacity = info_.opacity;
                    gpuObjects.push_back(object);

                    GPUBasicShapeObject ellipse;
                    ellipse.position[0] = info_.position.x;
                    ellipse.position[1] = info_.position.y;
                    ellipse.size[0]     = info_.size.x;
                    ellipse.size[1]     = info_.size.y;
                    ellipse.origin[0]   = info_.origin.x.value_or(std::floor(info_.size.x / 2.0f));
                    ellipse.origin[1]   = info_.origin.y.value_or(std::floor(info_.size.y / 2.0f));
                    ellipse.angle       = info_.angle.radians();
                    px::Vec4f color     = info_.color.normalize();
                    ellipse.color[0]    = color.x;
                    ellipse.color[1]    = color.y;
                    ellipse.color[2]    = color.z;
                    ellipse.color[3]    = color.w;
                    gpuShapes.push_back(ellipse);
                },
                [&gpuObjects, &gpuShapes](const LineDrawInfo &info_)
                {
                    GPUDrawObject object;
                    object.type    = 1;
                    object.index   = (int)gpuShapes.size();
                    object.filter  = info_.filter;
                    object.opacity = info_.opacity;
                    gpuObjects.push_back(object);

                    GPUBasicShapeObject rectangle;
                    // rectangle.position[0] = info_.position.x;
                    // rectangle.position[1] = info_.position.y;
                    // rectangle.size[0]     = info_.size.x;
                    // rectangle.size[1]     = info_.size.y;
                    // rectangle.origin[0]   = info_.origin.x.value_or(std::floor(info_.size.x / 2.0f));
                    // rectangle.origin[1]   = info_.origin.y.value_or(std::floor(info_.size.y / 2.0f));
                    // rectangle.angle       = info_.angle.radians();
                    // px::Vec4f color       = info_.color.normalize();
                    // rectangle.color[0]    = color.x;
                    // rectangle.color[1]    = color.y;
                    // rectangle.color[2]    = color.z;
                    // rectangle.color[3]    = color.w;
                    gpuShapes.push_back(rectangle);
                }},
            object.type);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    m_objectBuffer.upload(copyPass, gpuObjects);
    m_textureBuffer.upload(copyPass, gpuTextures);
    m_shapeBuffer.upload(copyPass, gpuShapes);

    SDL_EndGPUCopyPass(copyPass);
}
