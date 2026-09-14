struct TextureObject
{
	float2 uvPos;
	float2 uvSize;
    float2 position;
    float2 size;
    float angle;
    float _pad[3];
};

struct RectangleObject
{
    float2 position;
    float2 size;
    float angle;
    float _pad[3];
};

struct EllipseObject
{
    float2 position;
    float2 size;
	float angle;
    float _pad[3];
};

struct DrawObject
{
    int type;
	int _pad[3];
	TextureObject texture;
	RectangleObject rectangle;
	EllipseObject ellipse;
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
            return atlasTexture.Sample(atlasSampler, object.texture.uvPos + input.uv * object.texture.uvSize);
        }
        break;
        case 1:
        {
            color = float4(input.uv, 1.0f, 1.0f);
        }
        break;
        case 2:
        {
            float2 uv = input.uv - 0.5f;

            if (uv.x * uv.x + uv.y * uv.y < 0.25f)
            {
                color = float4(input.uv, 1.0f, 1.0f);
            }
        }
        break;
    }

    return color;
}
