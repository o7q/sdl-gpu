struct Input
{
    float4 screenPos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D<float4> layer0Texture : register(t0, space2);
SamplerState layer0Sampler : register(s0, space2);

Texture2D<float4> layer1Texture : register(t1, space2);
SamplerState layer1Sampler : register(s1, space2);

cbuffer Uniforms : register(b0, space3)
{
    uint layerBlendMode;
};

float4 alphaOver(float4 a, float4 b)
{
    float4 color;
    color.r = lerp(a.r, b.r, b.a);
    color.g = lerp(a.g, b.g, b.a);
    color.b = lerp(a.b, b.b, b.a);
    color.a = lerp(b.a, 1.0f, a.a);
    return color;
}

float4 main(Input input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    input.uv.y = 1.0f - input.uv.y;

    float4 color0 = layer0Texture.Sample(layer0Sampler, input.uv);
    float4 color1 = layer1Texture.Sample(layer1Sampler, input.uv);

    float3 color0_rgb = color0.rgb;
    float3 color1_rgb = color1.rgb;

    float3 color_min = float3(0, 0, 0);
    float3 color_max = float3(1, 1, 1);

    switch (layerBlendMode)
    {
        // replace
        case 0:
        {
            color = color1;
        }
        break;
        // alpha-over
        case 1:
        {
            color = alphaOver(color0, color1);
        }
        break;
        // add
        case 2:
        {
            float3 composited = clamp(color0_rgb + color1_rgb, color_min, color_max);
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // subtract
        case 3:
        {
            float3 composited = clamp(color0_rgb - color1_rgb, color_min, color_max);
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // multiply
        case 4:
        {
            float3 composited = clamp(color0_rgb * color1_rgb, color_min, color_max);
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // divide
        case 5:
        {
            float3 composited = clamp(color0_rgb / color1_rgb, color_min, color_max);
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // and
        case 6:
        {
            float3 composited = (int3(color0_rgb * 255) & int3(color1_rgb * 255)) / 255.0f;
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // or
        case 7:
        {
            float3 composited = (int3(color0_rgb * 255) | int3(color1_rgb * 255)) / 255.0f;
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // xor
        case 8:
        {
            float3 composited = (int3(color0_rgb * 255) ^ int3(color1_rgb * 255)) / 255.0f;
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // lighten
        case 9:
        {
            float3 composited = max(color0_rgb, color1_rgb);
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
        // darken
        case 10:
        {
            float3 composited = min(color0_rgb, color1_rgb);
            color = alphaOver(color0, float4(composited, color1.a));
        }
        break;
    }

    return color;
}
