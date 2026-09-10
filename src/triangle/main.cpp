#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <iostream>

struct Vertex
{
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 1.0f;
};

SDL_GPUShader *loadVertexShader(SDL_GPUDevice *device)
{
    size_t vertexCodeSize;
    void *vertexCode = SDL_LoadFile("src/triangle/vertex.spv", &vertexCodeSize);
    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code                 = (Uint8 *)vertexCode;
    vertexInfo.code_size            = vertexCodeSize;
    vertexInfo.entrypoint           = "main";
    vertexInfo.format               = SDL_GPU_SHADERFORMAT_SPIRV;
    vertexInfo.stage                = SDL_GPU_SHADERSTAGE_VERTEX;
    vertexInfo.num_samplers         = 0;
    vertexInfo.num_storage_buffers  = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers  = 0;
    SDL_GPUShader *vertexShader     = SDL_CreateGPUShader(device, &vertexInfo);
    SDL_free(vertexCode);
    return vertexShader;
}

SDL_GPUShader *loadFragmentShader(SDL_GPUDevice *device)
{
    size_t fragmentCodeSize;
    void *fragmentCode = SDL_LoadFile("src/triangle/fragment.spv", &fragmentCodeSize);
    SDL_GPUShaderCreateInfo fragmentInfo{};
    fragmentInfo.code                 = (Uint8 *)fragmentCode;
    fragmentInfo.code_size            = fragmentCodeSize;
    fragmentInfo.entrypoint           = "main";
    fragmentInfo.format               = SDL_GPU_SHADERFORMAT_SPIRV;
    fragmentInfo.stage                = SDL_GPU_SHADERSTAGE_FRAGMENT;
    fragmentInfo.num_samplers         = 0;
    fragmentInfo.num_storage_buffers  = 0;
    fragmentInfo.num_storage_textures = 0;
    fragmentInfo.num_uniform_buffers  = 0;
    SDL_GPUShader *fragmentShader     = SDL_CreateGPUShader(device, &fragmentInfo);
    SDL_free(fragmentCode);
    return fragmentShader;
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

    std::array<Vertex, 3> tri{};
    // bottom left
    tri[0].x = -1.0f;
    tri[0].y = -1.0f;
    tri[0].r = 1.0f;
    // bottom right
    tri[1].x = 1.0f;
    tri[1].y = -1.0f;
    tri[1].g = 1.0f;
    // top
    tri[2].x = 0.0f;
    tri[2].y = 1.0f;
    tri[2].b = 1.0f;

    SDL_GPUShader *vertexShader   = loadVertexShader(device);
    SDL_GPUShader *fragmentShader = loadFragmentShader(device);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    pipelineInfo.vertex_shader   = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;
    pipelineInfo.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    SDL_GPUVertexBufferDescription vertexBufferDescriptions[1];
    vertexBufferDescriptions[0].slot                           = 0;
    vertexBufferDescriptions[0].input_rate                     = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    vertexBufferDescriptions[0].instance_step_rate             = 0;
    vertexBufferDescriptions[0].pitch                          = sizeof(Vertex);
    pipelineInfo.vertex_input_state.num_vertex_buffers         = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDescriptions;

    SDL_GPUVertexAttribute vertexAttributes[2];
    // a_position
    vertexAttributes[0].buffer_slot = 0;
    vertexAttributes[0].location    = 0;
    vertexAttributes[0].format      = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
    vertexAttributes[0].offset      = 0;
    // a_color
    vertexAttributes[1].buffer_slot                       = 0;
    vertexAttributes[1].location                          = 1;
    vertexAttributes[1].format                            = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
    vertexAttributes[1].offset                            = sizeof(float) * 3;
    pipelineInfo.vertex_input_state.num_vertex_attributes = 2;
    pipelineInfo.vertex_input_state.vertex_attributes     = vertexAttributes;

    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0]                                   = {};
    colorTargetDescriptions[0].blend_state.enable_blend          = true;
    colorTargetDescriptions[0].blend_state.color_blend_op        = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;
    colorTargetDescriptions[0].blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    colorTargetDescriptions[0].blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    colorTargetDescriptions[0].format                  = SDL_GetGPUSwapchainTextureFormat(device, window);
    pipelineInfo.target_info.num_color_targets         = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    SDL_GPUGraphicsPipeline *graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    SDL_ReleaseGPUShader(device, fragmentShader);
    SDL_ReleaseGPUShader(device, vertexShader);

    bool running = true;

    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size             = sizeof(tri);
    bufferInfo.usage            = SDL_GPU_BUFFERUSAGE_VERTEX;
    SDL_GPUBuffer *vertexBuffer = SDL_CreateGPUBuffer(device, &bufferInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo{};
    transferInfo.size                     = sizeof(tri);
    transferInfo.usage                    = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    SDL_GPUTransferBuffer *transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);

    Vertex *data = (Vertex *)SDL_MapGPUTransferBuffer(device, transferBuffer, false);
    data[0]      = tri[0];
    data[1]      = tri[1];
    data[2]      = tri[2];
    SDL_UnmapGPUTransferBuffer(device, transferBuffer);

    SDL_GPUCommandBuffer *commandBuffer = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copyPass           = SDL_BeginGPUCopyPass(commandBuffer);

    SDL_GPUTransferBufferLocation location{};
    location.transfer_buffer = transferBuffer;
    location.offset          = 0;

    SDL_GPUBufferRegion region{};
    region.buffer = vertexBuffer;
    region.size   = sizeof(tri);
    region.offset = 0;

    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(commandBuffer);

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

        SDL_GPUTexture *swapchainTexture;
        Uint32 width, height;
        SDL_WaitAndAcquireGPUSwapchainTexture(cb, window, &swapchainTexture, &width, &height);

        if (!swapchainTexture)
        {
            SDL_SubmitGPUCommandBuffer(cb);
            continue;
        }

        SDL_GPUColorTargetInfo colorTargetInfo{};
        colorTargetInfo.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
        colorTargetInfo.load_op     = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op    = SDL_GPU_STOREOP_STORE;
        colorTargetInfo.texture     = swapchainTexture;

        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(cb, &colorTargetInfo, 1, nullptr);
        SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

        SDL_GPUBufferBinding bufferBindings[1];
        bufferBindings[0].buffer = vertexBuffer;
        bufferBindings[0].offset = 0;

        SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1);

        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        SDL_EndGPURenderPass(renderPass);
        SDL_SubmitGPUCommandBuffer(cb);
    }

    SDL_ReleaseGPUTransferBuffer(device, transferBuffer);
    SDL_ReleaseGPUBuffer(device, vertexBuffer);

    SDL_DestroyGPUDevice(device);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}
