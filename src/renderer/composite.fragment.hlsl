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

float4 alphaOver(float4 bottom, float4 top)
{
    float newAlpha = top.a + bottom.a * (1.0f - top.a);
    float3 newColor = top.rgb + bottom.rgb * (1.0f - top.a);
    return float4(newColor, newAlpha);
}

float4 main(Input input) : SV_TARGET
{
    input.uv.y = 1.0f - input.uv.y;

    float4 color0 = layer0Texture.Sample(layer0Sampler, input.uv);
    float4 color1 = layer1Texture.Sample(layer1Sampler, input.uv);

    float3 color0_rgb = color0.rgb / color0.a;
    float3 color1_rgb = color1.rgb / color0.a;

    float3 color_min = float3(0, 0, 0);
    float3 color_max = float3(1, 1, 1);

    switch (layerBlendMode)
    {
        // replace
        case 0:
        {
            return color1;
        }
        // alpha-over
        case 1:
        {
            return alphaOver(color0, color1);
        }
        // add
        case 2:
        {
            float3 composited = clamp(color0_rgb + color1_rgb, color_min, color_max);
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // subtract
        case 3:
        {
            float3 composited = clamp(color0_rgb - color1_rgb, color_min, color_max);
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // multiply
        case 4:
        {
            float3 composited = clamp(color0_rgb * color1_rgb, color_min, color_max);
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // divide
        case 5:
        {
            float3 composited = clamp(color0_rgb / color1_rgb, color_min, color_max);
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // and
        case 6:
        {
            float3 composited = (int3(color0_rgb * 255) & int3(color1_rgb * 255)) / 255.0f;
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // or
        case 7:
        {
            float3 composited = (int3(color0_rgb * 255) | int3(color1_rgb * 255)) / 255.0f;
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // xor
        case 8:
        {
            float3 composited = (int3(color0_rgb * 255) ^ int3(color1_rgb * 255)) / 255.0f;
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // lighten
        case 9:
        {
            float3 composited = max(color0_rgb, color1_rgb);
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
        // darken
        case 10:
        {
            float3 composited = min(color0_rgb, color1_rgb);
            return alphaOver(color0, float4(composited * color1.a, color1.a));
        }
    }

    return color0;
}
