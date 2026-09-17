#pragma once

struct GPUTextureObject
{
    float atlasPositionNormal[2]; // 0
    float atlasSizeNormal[2];     // 8
    int atlasIndex;               // 16
    float angle;                  // 20
    float position[2];            // 24
    float size[2];                // 32
    float origin[2];              // 40
    float cropTopLeft[2];         // 48
    float cropBottomRight[2];     // 56
};

struct GPURectangleObject
{
    float position[2]; // 0
    float size[2];     // 8
    float origin[2];   // 16
    float angle;       // 24
    float _pad0;       // 28
    float color[4];    // 32
};

struct GPUEllipseObject
{
    float position[2]; // 0
    float size[2];     // 8
    float origin[2];   // 16
    float angle;       // 24
    float _pad0;       // 28
    float color[4];    // 32
};

struct GPULineObject
{
    float a[2];      // 0
    float b[2];      // 8
    float color[4];  // 16
    float thickness; // 32
};

struct GPUDrawObject
{
    int type;      // 0
    int index;     // 4
    float opacity; // 8
    int filter;    // 12
};
