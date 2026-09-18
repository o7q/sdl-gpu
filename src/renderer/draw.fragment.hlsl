struct TextureObject
{
    float2 atlasPositionNormal;
    float2 atlasSizeNormal;
    int atlasIndex;
    float angle;
    float2 position;
    float2 size;
    float2 origin;
    float2 cropTopLeft;
    float2 cropBottomRight;
};

struct GPUBasicShapeObject
{
    float2 position;
    float2 size;
    float2 origin;
    int fill;
    float outlineThickness;
    float4 color;
    float angle;
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

Texture2DArray<float4> atlasTextures : register(t0, space2);
SamplerState atlasSampler : register(s0, space2);

StructuredBuffer<DrawObject> objectBuffer : register(t1, space2);
StructuredBuffer<TextureObject> textureBuffer : register(t2, space2);
StructuredBuffer<GPUBasicShapeObject> shapeBuffer : register(t3, space2);

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
                float3 uv3 = float3(uv, (float)tex.atlasIndex);
                color = atlasTextures.Sample(atlasSampler, uv3);
            }
        }
        break;
        case 1:
        {
            GPUBasicShapeObject shape = shapeBuffer[object.index];
            color = shape.color;
        }
        break;
        case 2:
        {
            GPUBasicShapeObject shape = shapeBuffer[object.index];

            float2 uv = input.uv - 0.5f;

            if (shape.fill)
            {
                if (uv.x * uv.x + uv.y * uv.y < 0.25f)
                {
                    color = shape.color;
                }
            }
            else {     
                if (uv.x * uv.x + uv.y * uv.y < 0.25f && uv.x * uv.x + uv.y * uv.y > 0.2444444f)
                {
                    color = shape.color;
                }
            }
        }
        break;
    }

    switch (object.filter)
    {
        case 1:
        color.r /= color.a;
        color.g /= color.a;
        color.b /= color.a;
        color.r = 1.0f - color.r;
        color.g = 1.0f - color.g;
        color.b = 1.0f - color.b;
        color.r *= color.a;
        color.g *= color.a;
        color.b *= color.a;
        break;
    }

    color.a *= object.opacity;

    return color;
}
