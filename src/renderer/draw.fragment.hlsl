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
    int index;
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
StructuredBuffer<TextureObject> textureBuffer : register(t2, space2);
StructuredBuffer<RectangleObject> rectangleBuffer : register(t3, space2);
StructuredBuffer<EllipseObject> ellipseBuffer : register(t4, space2);

float4 main(Input input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    DrawObject object = objectBuffer[input.objectIndex];

    switch (object.type)
    {
        case 0:
        {
            TextureObject tex = textureBuffer[object.index];

            if (input.uv.x >= tex.cropTopLeft.x && input.uv.x <= 1.0f - tex.cropBottomRight.x &&
            input.uv.y >= tex.cropTopLeft.y && input.uv.y <= 1.0f - tex.cropBottomRight.y)
            {   
                float2 uv = tex.atlasPositionNormal + input.uv * tex.atlasSizeNormal;
                color = atlasTexture.Sample(atlasSampler, uv);
            }
        }
        break;
        case 1:
        {
            RectangleObject rect = rectangleBuffer[object.index];
            color = rect.color;
        }
        break;
        case 2:
        {
            EllipseObject ellipse = ellipseBuffer[object.index];

            float2 uv = input.uv - 0.5f;

            if (uv.x * uv.x + uv.y * uv.y < 0.25f)
            {
                color = ellipse.color;
            }
        }
        break;
    }

    switch (object.filter)
    {
        case 1:
        {
            color.r = 1.0f - color.r;
            color.g = 1.0f - color.g;
            color.b = 1.0f - color.b;
        }
        break;
    }

    color.a *= object.opacity;

    return color;
}
