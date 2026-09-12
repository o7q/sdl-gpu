Texture2D<float4> Texture : register(t0, space2);
SamplerState Sampler : register(s0, space2);

float4 main(float2 TexCoord : TEXCOORD0) : SV_Target0
{
    float4 color = Texture.Sample(Sampler, TexCoord);

    // color.x = 1.0f - color.x;
    // color.y = 1.0f - color.y;
    // color.z = 1.0f - color.z;

	return color;
}
