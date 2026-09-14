struct TextureObject
{
    float2 atlasPositionNormal;
    float2 atlasSizeNormal;

    float2 position;
    float2 size;
    float2 origin;
    float angle;
    float2 cropTopLeft;
    float2 cropBottomRight;
};

struct RectangleObject
{
    float2 position;
    float2 size;
    float2 origin;
    float angle;
    float4 color;
};

struct EllipseObject
{
    float2 position;
    float2 size;
    float2 origin;
    float angle;
    float4 color;
};

struct DrawObject
{
    int type;
    TextureObject texture;
    RectangleObject rectangle;
    EllipseObject ellipse;
    float opacity;
    int filter;
};

struct Input
{
    uint objectIndex : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

Texture2D<float4> atlasTexture : register(t0, space2);
SamplerState atlasSampler : register(s0, space2);

StructuredBuffer<DrawObject> objectBuffer : register(t1, space2);

float4 main(Input input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    DrawObject object = objectBuffer[input.objectIndex];

    switch (object.type)
    {
        case 0:
        {
            if (input.uv.x >= object.texture.cropTopLeft.x && input.uv.x <= 1.0f - object.texture.cropBottomRight.x &&
            input.uv.y >= object.texture.cropTopLeft.y && input.uv.y <= 1.0f - object.texture.cropBottomRight.y)
            {   
                float2 uv = object.texture.atlasPositionNormal + input.uv * object.texture.atlasSizeNormal;
                color = atlasTexture.Sample(atlasSampler, uv);
            }
        }
        break;
        case 1:
        {
            color = object.rectangle.color;
        }
        break;
        case 2:
        {
            float2 uv = input.uv - 0.5f;

            if (uv.x * uv.x + uv.y * uv.y < 0.25f)
            {
                color = float4(input.uv, 0.0f, 1.0f);
            }
        }
        break;
    }

    switch (object.filter)
    {
        case 1:
            color.r = 1.0f - color.r;
            color.g = 1.0f - color.g;
            color.b = 1.0f - color.b;
            break;
    }

    color.w *= object.opacity;

    return color;
}
