#pragma once

struct GPUTextureObject
{
    float atlasPositionNormal[2];
    float atlasSizeNormal[2];
    int atlasIndex;
    float _pad0;
    float position[2];
    float size[2];
    float origin[2];
    float angle;
    float _pad1;
    float cropTopLeft[2];
    float cropBottomRight[2];
};

struct GPURectangleObject
{
    float position[2];
    float size[2];
    float origin[2];
    float angle;
    float _pad0;
    float color[4];
};

struct GPUEllipseObject
{
    float position[2];
    float size[2];
    float origin[2];
    float angle;
    float _pad0;
    float color[4];
};

struct GPULineObject
{
    float a[2];
    float b[2];
    float thickness;
    float _pad0;
    float color[4];
};

struct GPUDrawObject
{
    int type;
    int index;
    float opacity;
    int filter;
};
