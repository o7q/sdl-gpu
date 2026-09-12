#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_thread.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <cstring>
#include <iostream>

struct Vec2
{
    float x = 0.0f, y = 0.0f;
};

struct Sprite
{
    Vec2 position;
    Vec2 size;
    float angle = 0.0f;
    float _pad1[3];
    float r = 1.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

struct SpriteUniform
{
    Sprite sprite;
    Vec2 screenSize;
};

SDL_GPUShader *loadShader(SDL_GPUDevice *device, const std::string &path, SDL_GPUShaderStage stage,
                          SDL_GPUShaderFormat format, Uint32 samplerCount, Uint32 storageBufferCount,
                          Uint32 storageTextureCount, Uint32 uniformCount)
{
    size_t codeSize;
    void *code = SDL_LoadFile(path.data(), &codeSize);
    SDL_GPUShaderCreateInfo info{};
    info.code                 = (Uint8 *)code;
    info.code_size            = codeSize;
    info.entrypoint           = "main";
    info.format               = format;
    info.stage                = stage;
    info.num_samplers         = samplerCount;
    info.num_storage_buffers  = storageBufferCount;
    info.num_storage_textures = storageTextureCount;
    info.num_uniform_buffers  = uniformCount;
    SDL_GPUShader *shader     = SDL_CreateGPUShader(device, &info);
    SDL_free(code);
    return shader;
}

SDL_GPUGraphicsPipeline *createSpritePipeline(SDL_GPUDevice *device, SDL_Window *window)
{
    SDL_GPUShader *vertexShader =
        loadShader(device, "src/sprites/compiled/sprite.vertex.spv", SDL_GPU_SHADERSTAGE_VERTEX,
                   SDL_GPU_SHADERFORMAT_SPIRV, 0, 0, 0, 1);
    SDL_GPUShader *fragmentShader =
        loadShader(device, "src/sprites/compiled/sprite.fragment.spv", SDL_GPU_SHADERSTAGE_FRAGMENT,
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
    colorTargetDescriptions.format                     = SDL_GetGPUSwapchainTextureFormat(device, window);
    pipelineInfo.target_info.num_color_targets         = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDescriptions;

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    SDL_ReleaseGPUShader(device, fragmentShader);
    SDL_ReleaseGPUShader(device, vertexShader);

    return pipeline;
}

SDL_GPUGraphicsPipeline *createRenderPipeline(SDL_GPUDevice *device, SDL_Window *window)
{
    SDL_GPUShader *vertexShader =
        loadShader(device, "src/sprites/compiled/screen.vertex.spv", SDL_GPU_SHADERSTAGE_VERTEX,
                   SDL_GPU_SHADERFORMAT_SPIRV, 0, 0, 0, 1);
    SDL_GPUShader *fragmentShader =
        loadShader(device, "src/sprites/compiled/screen.fragment.spv", SDL_GPU_SHADERSTAGE_FRAGMENT,
                   SDL_GPU_SHADERFORMAT_SPIRV, 1, 0, 0, 0);

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
    colorTargetDescriptions.format                     = SDL_GetGPUSwapchainTextureFormat(device, window);
    pipelineInfo.target_info.num_color_targets         = 1;
    pipelineInfo.target_info.color_target_descriptions = &colorTargetDescriptions;

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    SDL_ReleaseGPUShader(device, fragmentShader);
    SDL_ReleaseGPUShader(device, vertexShader);

    return pipeline;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("sdl gpu", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cout << "Failed to create window!" << std::endl;
        return 1;
    }

    SDL_GPUDevice *device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device)
    {
        std::cout << "Failed to create device!" << std::endl;
        return 1;
    }

    if (!SDL_ClaimWindowForGPUDevice(device, window))
    {
        std::cout << "Error claiming window!" << std::endl;
        return 1;
    }

    SDL_GPUGraphicsPipeline *spritePipeline = createSpritePipeline(device, window);
    SDL_GPUGraphicsPipeline *renderPipeline = createRenderPipeline(device, window);

    SDL_SetGPUSwapchainParameters(device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                  SDL_GPU_PRESENTMODE_IMMEDIATE);

    Uint64 startTicks = SDL_GetTicks();
    int frameCount    = 0;

    bool running = true;

    Uint32 drawWidth  = 100;
    Uint32 drawHeight = 64;

    SDL_GPUTextureCreateInfo textureInfo{};
    textureInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    textureInfo.width                = drawWidth;
    textureInfo.height               = drawHeight;
    textureInfo.num_levels           = 1;
    textureInfo.layer_count_or_depth = 1;
    textureInfo.usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER;
    SDL_GPUTexture *pixelTarget      = SDL_CreateGPUTexture(device, &textureInfo);

    SDL_GPUSamplerCreateInfo samplerInfo{};
    samplerInfo.min_filter     = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mag_filter     = SDL_GPU_FILTER_NEAREST;
    samplerInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    SDL_GPUSampler *sampler    = SDL_CreateGPUSampler(device, &samplerInfo);

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                running = false;
                break;
            }
        }

        SDL_GPUCommandBuffer *cb = SDL_AcquireGPUCommandBuffer(device);

        // RENDER
        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color   = {0.1f, 0.1f, 0.1f, 1.0f};
        colorTargetInfo.load_op       = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op      = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture       = pixelTarget;
        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(cb, &colorTargetInfo, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, spritePipeline);

        SpriteUniform uniform;
        uniform.sprite.position = {10, 10};
        uniform.sprite.size     = {30, 30};
        uniform.sprite.angle = SDL_GetTicks() / 1000.0f;
        uniform.screenSize      = {(float)drawWidth, (float)drawHeight};

        SDL_PushGPUVertexUniformData(cb, 0, &uniform, sizeof(uniform));
        SDL_DrawGPUPrimitives(renderPass, 6, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass);
        //

        // BLIT
        SDL_GPUTexture *swapchainTexture;
        Uint32 width, height;
        SDL_WaitAndAcquireGPUSwapchainTexture(cb, window, &swapchainTexture, &width, &height);

        if (!swapchainTexture)
        {
            SDL_SubmitGPUCommandBuffer(cb);
            continue;
        }

        SDL_GPUColorTargetInfo colorTargetInfo2{};
        colorTargetInfo2.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        colorTargetInfo2.load_op     = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo2.store_op    = SDL_GPU_STOREOP_STORE;
        colorTargetInfo2.texture     = swapchainTexture;

        SDL_GPURenderPass *renderPass2 = SDL_BeginGPURenderPass(cb, &colorTargetInfo2, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, renderPipeline);

        SDL_GPUTextureSamplerBinding textureBinding;
        textureBinding.texture = pixelTarget;
        textureBinding.sampler = sampler;
        SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);

        float ratios[2]{(float)width / (float)height, (float)drawWidth / (float)drawHeight};

        // std::cout << ratios[0] << std::endl;
        // std::cout << ratios[1] << std::endl;
        // std::cout << std::endl;

        SDL_PushGPUVertexUniformData(cb, 0, &ratios, sizeof(ratios));
        SDL_DrawGPUPrimitives(renderPass2, 6, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass2);

        //

        SDL_SubmitGPUCommandBuffer(cb);

        Uint64 currentTicks = SDL_GetTicks();
        frameCount++;
        if (currentTicks - startTicks >= 1000)
        {
            std::cout << frameCount << std::endl;
            frameCount = 0;
            startTicks = currentTicks;
        }
    }

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
