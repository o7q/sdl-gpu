#include "renderer.hpp"

#include "renderer/shader.hpp"

void Renderer::init_composite_pipeline()
{
    SDL_GPUShader *vertexShader =
        loadShader(m_device, "src/renderer/compiled/composite.vertex.spv", SDL_GPU_SHADERSTAGE_VERTEX,
                   SDL_GPU_SHADERFORMAT_SPIRV, 0, 0, 0, 0);
    SDL_GPUShader *fragmentShader =
        loadShader(m_device, "src/renderer/compiled/composite.fragment.spv", SDL_GPU_SHADERSTAGE_FRAGMENT,
                   SDL_GPU_SHADERFORMAT_SPIRV, 2, 0, 0, 1);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.vertex_shader   = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUColorTargetDescription colorTargetDescriptions{};
    colorTargetDescriptions.blend_state.enable_blend   = true;
    colorTargetDescriptions.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;

    colorTargetDescriptions.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    colorTargetDescriptions.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;

    colorTargetDescriptions.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    colorTargetDescriptions.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    colorTargetDescriptions.format                     = SDL_GetGPUSwapchainTextureFormat(m_device, m_window);
    pipelineInfo.target_info.num_color_targets         = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDescriptions;

    m_compositePipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);

    SDL_ReleaseGPUShader(m_device, fragmentShader);
    SDL_ReleaseGPUShader(m_device, vertexShader);
}
