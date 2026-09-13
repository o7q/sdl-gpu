#include "renderer.hpp"
#include "shader.hpp"

void Renderer::init_draw_pipeline()
{
    SDL_GPUShader *vertexShader =
        loadShader(m_device, "src/compiled/draw.vertex.spv", SDL_GPU_SHADERSTAGE_VERTEX,
                   SDL_GPU_SHADERFORMAT_SPIRV, 0, 3, 0, 1);
    SDL_GPUShader *fragmentShader =
        loadShader(m_device, "src/compiled/draw.fragment.spv", SDL_GPU_SHADERSTAGE_FRAGMENT,
                   SDL_GPU_SHADERFORMAT_SPIRV, 0, 0, 0, 0);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.vertex_shader   = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUColorTargetDescription colorTargetDescriptions{};
    colorTargetDescriptions.blend_state.enable_blend          = true;
    colorTargetDescriptions.blend_state.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions.blend_state.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions.format                     = SDL_GetGPUSwapchainTextureFormat(m_device, m_window);
    pipelineInfo.target_info.num_color_targets         = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDescriptions;

    m_drawPipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);

    SDL_ReleaseGPUShader(m_device, fragmentShader);
    SDL_ReleaseGPUShader(m_device, vertexShader);
}

void Renderer::init_blit_pipeline()
{
    SDL_GPUShader *vertexShader =
        loadShader(m_device, "src/compiled/blit.vertex.spv", SDL_GPU_SHADERSTAGE_VERTEX,
                   SDL_GPU_SHADERFORMAT_SPIRV, 0, 0, 0, 1);
    SDL_GPUShader *fragmentShader =
        loadShader(m_device, "src/compiled/blit.fragment.spv", SDL_GPU_SHADERSTAGE_FRAGMENT,
                   SDL_GPU_SHADERFORMAT_SPIRV, 1, 0, 0, 1);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.vertex_shader   = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUColorTargetDescription colorTargetDescriptions{};
    colorTargetDescriptions.blend_state.enable_blend          = true;
    colorTargetDescriptions.blend_state.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions.blend_state.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions.format                     = SDL_GetGPUSwapchainTextureFormat(m_device, m_window);
    pipelineInfo.target_info.num_color_targets         = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDescriptions;

    m_blitPipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipelineInfo);

    SDL_ReleaseGPUShader(m_device, fragmentShader);
    SDL_ReleaseGPUShader(m_device, vertexShader);
}

void Renderer::init_draw_texture()
{
    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width                = m_width;
    textureInfo.height               = m_height;
    textureInfo.num_levels           = 1;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    m_drawTexture                    = SDL_CreateGPUTexture(m_device, &textureInfo);

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter     = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter     = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    m_sampler                  = SDL_CreateGPUSampler(m_device, &samplerInfo);
}
