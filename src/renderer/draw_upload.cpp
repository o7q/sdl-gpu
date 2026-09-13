#include "renderer.hpp"
#include "variant_bp.hpp"
#include <algorithm>

void Renderer::draw_upload_queue(std::vector<DrawInfo> &queue, SDL_GPUCommandBuffer *commandBuffer)
{
    std::sort(queue.begin(), queue.end(),
              [](const DrawInfo &a, const DrawInfo &b) { return a.index < b.index; });

    std::vector<DrawObject> objects;
    std::vector<RectangleObject> rectObjects;
    std::vector<CircleObject> circleObjects;

    for (DrawInfo &info : queue)
    {
        std::visit(overloaded{[&objects, &rectObjects](const RectangleDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.objectType = 0;
                                  object.index      = (int)rectObjects.size();

                                  RectangleObject rectObject;
                                  rectObject.position = info_.position;
                                  rectObject.size     = info_.size;
                                  rectObject.angle    = info_.angle;

                                  objects.push_back(object);
                                  rectObjects.push_back(rectObject);
                              },
                              [&objects, &circleObjects](const CircleDrawInfo &info_)
                              {
                                  DrawObject object;
                                  object.objectType = 1;
                                  object.index      = (int)circleObjects.size();

                                  CircleObject circleObject;
                                  circleObject.position = info_.position;
                                  circleObject.radius   = info_.radius;

                                  objects.push_back(object);
                                  circleObjects.push_back(circleObject);
                              }},
                   info.info);
    }

    SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    upload_object_buffer(copyPass, &m_objectBuffer, objects.data(), objects.size() * sizeof(DrawObject));
    upload_object_buffer(copyPass, &m_rectangleBuffer, rectObjects.data(),
                         rectObjects.size() * sizeof(RectangleObject));
    upload_object_buffer(copyPass, &m_circleBuffer, circleObjects.data(),
                         circleObjects.size() * sizeof(CircleObject));

    SDL_EndGPUCopyPass(copyPass);
}