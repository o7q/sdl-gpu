#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <iostream>

struct PositionTextureVertex
{
    float x, y, z;
    float u, v;
};

SDL_GPUShader *loadShader(const std::string &path, SDL_GPUDevice *device, SDL_GPUShaderStage stage,
                          int samplerCount)
{
    size_t codeSize;
    void *code = SDL_LoadFile(path.c_str(), &codeSize);
    SDL_GPUShaderCreateInfo shaderInfo{};
    shaderInfo.code_size            = codeSize;
    shaderInfo.code                 = (Uint8 *)code;
    shaderInfo.entrypoint           = "main";
    shaderInfo.format               = SDL_GPU_SHADERFORMAT_SPIRV;
    shaderInfo.stage                = stage;
    shaderInfo.num_samplers         = samplerCount;
    shaderInfo.num_storage_buffers  = 0;
    shaderInfo.num_storage_textures = 0;
    shaderInfo.num_uniform_buffers  = 0;
    SDL_GPUShader *shader           = SDL_CreateGPUShader(device, &shaderInfo);
    SDL_free(code);
    return shader;
}

SDL_GPUGraphicsPipeline *createPipeline(SDL_GPUDevice *device, SDL_Window *window,
                                        SDL_GPUShader *vertexShader, SDL_GPUShader *fragmentShader)
{
    SDL_GPUGraphicsPipelineCreateInfo createInfo{};

    // target_info
    createInfo.target_info.num_color_targets = 1;
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0]                       = {};
    colorTargetDescriptions[0].format                = SDL_GetGPUSwapchainTextureFormat(device, window);
    createInfo.target_info.color_target_descriptions = colorTargetDescriptions;
    //

    // vertex_input_state
    createInfo.vertex_input_state.num_vertex_buffers = 1;
    SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
    vertexBufferDescriptions[0].slot                         = 0;
    vertexBufferDescriptions[0].input_rate                   = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDescriptions[0].instance_step_rate           = 0;
    vertexBufferDescriptions[0].pitch                        = sizeof(PositionTextureVertex);
    createInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;

    createInfo.vertex_input_state.num_vertex_attributes = 2;
    SDL_GPUVertexAttribute vertexAttributes[2];
    vertexAttributes[0].buffer_slot                 = 0;
    vertexAttributes[0].format                      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].location                    = 0;
    vertexAttributes[0].offset                      = 0;
    vertexAttributes[1].buffer_slot                 = 0;
    vertexAttributes[1].format                      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
    vertexAttributes[1].location                    = 1;
    vertexAttributes[1].offset                      = sizeof(float) * 3;
    createInfo.vertex_input_state.vertex_attributes = vertexAttributes;
    //

    createInfo.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    createInfo.vertex_shader   = vertexShader;
    createInfo.fragment_shader = fragmentShader;

    SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(device, &createInfo);
    return pipeline;
}

SDL_GPUSampler *createSampler(SDL_GPUDevice *device)
{
    SDL_GPUSamplerCreateInfo createInfo{};
    createInfo.min_filter     = SDL_GPU_FILTER_NEAREST;
    createInfo.mag_filter     = SDL_GPU_FILTER_NEAREST;
    createInfo.mipmap_mode    = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    createInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
    createInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT;
    createInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    SDL_GPUSampler *sampler   = SDL_CreateGPUSampler(device, &createInfo);
    return sampler;
}

SDL_GPUBuffer *createVertexBuffer(SDL_GPUDevice *device)
{
    SDL_GPUBufferCreateInfo createInfo{};
    createInfo.usage      = SDL_GPU_BUFFERUSAGE_VERTEX;
    createInfo.size       = sizeof(PositionTextureVertex) * 4;
    SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(device, &createInfo);
    return buffer;
}

SDL_GPUBuffer *createIndexBuffer(SDL_GPUDevice *device)
{
    SDL_GPUBufferCreateInfo createInfo{};
    createInfo.usage      = SDL_GPU_BUFFERUSAGE_INDEX;
    createInfo.size       = sizeof(Uint16) * 6;
    SDL_GPUBuffer *buffer = SDL_CreateGPUBuffer(device, &createInfo);
    return buffer;
}

SDL_GPUTexture *createTexture(SDL_GPUDevice *device, SDL_Surface *surface)
{
    SDL_GPUTextureCreateInfo createInfo{};
    createInfo.type                 = SDL_GPU_TEXTURETYPE_2D;
    createInfo.format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    createInfo.width                = surface->w;
    createInfo.height               = surface->h;
    createInfo.layer_count_or_depth = 1;
    createInfo.num_levels           = 1;
    createInfo.usage                = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    SDL_GPUTexture *texture         = SDL_CreateGPUTexture(device, &createInfo);
    return texture;
}

SDL_GPUTransferBuffer *createQuadTransferBuffer(SDL_GPUDevice *device)
{
    SDL_GPUTransferBufferCreateInfo createInfo{};
    createInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    // 4 vertices for the quad, and 6 indices that map each vertex to 2 triangles
    createInfo.size               = sizeof(PositionTextureVertex) * 4 + sizeof(Uint16) * 6;
    SDL_GPUTransferBuffer *buffer = SDL_CreateGPUTransferBuffer(device, &createInfo);

    PositionTextureVertex *transferData =
        (PositionTextureVertex *)SDL_MapGPUTransferBuffer(device, buffer, false);
    transferData[0] = {-1, 1, 0, 0, 0};
    transferData[1] = {1, 1, 0, 1, 0};
    transferData[2] = {1, -1, 0, 1, 1};
    transferData[3] = {-1, -1, 0, 0, 1};

    Uint16 *indexData = (Uint16 *)(transferData + 4);
    indexData[0]      = 0;
    indexData[1]      = 1;
    indexData[2]      = 2;
    indexData[3]      = 0;
    indexData[4]      = 2;
    indexData[5]      = 3;
    SDL_UnmapGPUTransferBuffer(device, buffer);

    return buffer;
}

SDL_GPUTransferBuffer *createTextureTransferBuffer(SDL_GPUDevice *device, SDL_Surface *surface)
{
    SDL_GPUTransferBufferCreateInfo createInfo{};
    createInfo.usage              = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    createInfo.size               = surface->w * surface->h * 4;
    SDL_GPUTransferBuffer *buffer = SDL_CreateGPUTransferBuffer(device, &createInfo);

    Uint8 *textureTransferPtr = (Uint8 *)SDL_MapGPUTransferBuffer(device, buffer, false);
    SDL_memcpy(textureTransferPtr, surface->pixels, surface->w * surface->h * 4);
    SDL_UnmapGPUTransferBuffer(device, buffer);

    return buffer;
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

    SDL_Surface *surface = SDL_LoadPNG("src/texture/sparklecat.png");
    // SDL_Surface *surface = SDL_CreateSurface(5, 5, SDL_PIXELFORMAT_ABGR8888);
    Uint8 *pixels        = (Uint8 *)surface->pixels;
    pixels[0]            = 255;
    pixels[6 * 4]        = 255;
    pixels[12 * 4]       = 255;
    pixels[18 * 4]       = 255;
    pixels[24 * 4]       = 255;
    if (!surface)
    {
        std::cout << "Error loading image!" << std::endl;
        return 1;
    }
    SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ABGR8888);
    SDL_DestroySurface(surface);
    surface = converted;

    SDL_GPUShader *vertexShader = loadShader("src/texture/vertex.spv", device, SDL_GPU_SHADERSTAGE_VERTEX, 0);
    SDL_GPUShader *fragmentShader =
        loadShader("src/texture/fragment.spv", device, SDL_GPU_SHADERSTAGE_FRAGMENT, 1);

    SDL_GPUGraphicsPipeline *pipeline = createPipeline(device, window, vertexShader, fragmentShader);

    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    SDL_GPUSampler *sampler     = createSampler(device);
    SDL_GPUBuffer *vertexBuffer = createVertexBuffer(device);
    SDL_GPUBuffer *indexBuffer  = createIndexBuffer(device);

    SDL_GPUTexture *texture = createTexture(device, surface);
    SDL_SetGPUTextureName(device, texture, "Sparkle Cat");

    SDL_GPUTransferBuffer *quadTransferBuffer    = createQuadTransferBuffer(device);
    SDL_GPUTransferBuffer *textureTransferBuffer = createTextureTransferBuffer(device, surface);

    SDL_GPUCommandBuffer *uploadCmdBuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copyPass          = SDL_BeginGPUCopyPass(uploadCmdBuf);

    // quad vertices
    SDL_GPUTransferBufferLocation vertexSrc;
    vertexSrc.transfer_buffer = quadTransferBuffer;
    vertexSrc.offset          = 0;
    SDL_GPUBufferRegion vertexDest;
    vertexDest.buffer = vertexBuffer;
    vertexDest.offset = 0;
    vertexDest.size   = sizeof(PositionTextureVertex) * 4;
    SDL_UploadToGPUBuffer(copyPass, &vertexSrc, &vertexDest, false);

    // indices
    SDL_GPUTransferBufferLocation indexSrc;
    indexSrc.transfer_buffer = quadTransferBuffer;
    indexSrc.offset          = sizeof(PositionTextureVertex) * 4;
    SDL_GPUBufferRegion indexDest;
    indexDest.buffer = indexBuffer;
    indexDest.offset = 0;
    indexDest.size   = sizeof(Uint16) * 6;
    SDL_UploadToGPUBuffer(copyPass, &indexSrc, &indexDest, false);

    // texture
    SDL_GPUTextureTransferInfo textureSrc{};
    textureSrc.transfer_buffer = textureTransferBuffer;
    textureSrc.offset          = 0;
    SDL_GPUTextureRegion textureDest{};
    textureDest.texture = texture;
    textureDest.w       = surface->w;
    textureDest.h       = surface->h;
    textureDest.d       = 1;
    SDL_UploadToGPUTexture(copyPass, &textureSrc, &textureDest, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
    SDL_DestroySurface(surface);
    SDL_ReleaseGPUTransferBuffer(device, quadTransferBuffer);
    SDL_ReleaseGPUTransferBuffer(device, textureTransferBuffer);

    bool running = true;
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

        SDL_GPUCommandBuffer *cmdBuf = SDL_AcquireGPUCommandBuffer(device);

        SDL_GPUTexture *swapchainTexture;
        Uint32 width, height;
        SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuf, window, &swapchainTexture, &width, &height);

        if (!swapchainTexture)
        {
            continue;
        }

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.texture     = swapchainTexture;
        colorTargetInfo.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
        colorTargetInfo.load_op     = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op    = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(cmdBuf, &colorTargetInfo, 1, nullptr);

        SDL_BindGPUGraphicsPipeline(renderPass, pipeline);

        SDL_GPUBufferBinding vertexBinding;
        vertexBinding.buffer = vertexBuffer;
        vertexBinding.offset = 0;
        SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBinding, 1);

        SDL_GPUBufferBinding indexBinding;
        indexBinding.buffer = indexBuffer;
        indexBinding.offset = 0;
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        SDL_GPUTextureSamplerBinding textureBinding;
        textureBinding.texture = texture;
        textureBinding.sampler = sampler;
        SDL_BindGPUFragmentSamplers(renderPass, 0, &textureBinding, 1);
        SDL_DrawGPUIndexedPrimitives(renderPass, 4, 1, 0, 0, 0);

        SDL_EndGPURenderPass(renderPass);

        SDL_SubmitGPUCommandBuffer(cmdBuf);
    }

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
