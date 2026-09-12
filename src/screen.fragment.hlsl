Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

cbuffer Uniforms : register(b0, space3)
{
	float2 screenSize;
};

float2 normalToScreen(float2 normalPos)
{
	return normalPos * screenSize;
}

float2 screenToNormal(float2 screenPos)
{
	return screenPos / screenSize;
}

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{
    int radius = 10;

    int count = 0;
    float3 sum = float3(0, 0, 0);

    for (int y = -radius; y < radius; ++y)
    {
        for (int x = -radius; x < radius; ++x)
        {
            if (x * x + y * y > radius * radius)
            {
                continue;
            }

            float2 current = normalToScreen(TexCoord) + float2(x, y);

            float4 color = Texture.Sample(Sampler, screenToNormal(current));
            sum += color.xyz;
            ++count;
        }
    }

    sum /= (float)count;

    float4 color = Texture.Sample(Sampler, TexCoord);
    color = float4(color.xyz + sum, color.w);

	return color;
}
