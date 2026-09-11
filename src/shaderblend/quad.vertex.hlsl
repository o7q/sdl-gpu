struct VSInput
{
	float2 position : TEXCOORD0;
};

struct VSOutput
{
	float2 pos : TEXCOORD0;
	float4 position : SV_POSITION;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = float4(input.position.xy, 0.0f, 1.0f);
	output.pos = (input.position.xy + 1.0f) * 0.5f;
	return output;
}