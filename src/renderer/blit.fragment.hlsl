Texture2D<float4> drawTexture : register(t0, space2);
SamplerState drawSampler : register(s0, space2);

float4 main(float2 uv : TEXCOORD0) : SV_Target0
{
	return drawTexture.Sample(drawSampler, uv);
}
