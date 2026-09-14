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
    std::vector<TextureObject> textureObjects;
    std::vector<RectangleObject> rectObjects;
    std::vector<EllipseObject> ellipseObjects;

    for (DrawInfo &info : queue)
    {
        std::visit(overloaded{[this, &objects, &textureObjects](const TextureDrawInfo &info_)
                              {
                                  AtlasUVIndex index = m_atlas.getIndex(info_.name);

                                  DrawObject object;
                                  object.objectType = 0;
                                  object.index      = (int)textureObjects.size();
                                  objects.push_back(object);

                                  TextureObject textureObject;
                                  textureObject.uvPos    = index.location;
                                  textureObject.uvSize   = index.size;
                                  textureObject.position = info_.position;
                                  textureObject.size     = info_.size;
                                  textureObject.angle    = info_.angle;
                                  textureObjects.push_back(textureObject);
                              },
                              [&objects, &rectObjects](const RectangleDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.objectType = 1;
                                  object.index      = (int)rectObjects.size();
                                  objects.push_back(object);

                                  RectangleObject rectObject;
                                  rectObject.position = info_.position;
                                  rectObject.size     = info_.size;
                                  rectObject.angle    = info_.angle;
                                  rectObjects.push_back(rectObject);
                              },
                              [&objects, &ellipseObjects](const EllipseDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.objectType = 2;
                                  object.index      = (int)ellipseObjects.size();
                                  objects.push_back(object);

                                  EllipseObject ellipseObject;
                                  ellipseObject.position = info_.position;
                                  ellipseObject.size     = info_.size;
                                  ellipseObject.angle    = info_.angle;
                                  ellipseObjects.push_back(ellipseObject);
                              }},
                   info.info);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    upload_object_buffer(copyPass, &m_objectBuffer, objects.data(), objects.size() * sizeof(DrawObject));
    upload_object_buffer(copyPass, &m_textureBuffer, textureObjects.data(),
                         textureObjects.size() * sizeof(TextureObject));
    upload_object_buffer(copyPass, &m_rectangleBuffer, rectObjects.data(),
                         rectObjects.size() * sizeof(RectangleObject));
    upload_object_buffer(copyPass, &m_ellipseBuffer, ellipseObjects.data(),
                         ellipseObjects.size() * sizeof(EllipseObject));

    SDL_EndGPUCopyPass(copyPass);
}