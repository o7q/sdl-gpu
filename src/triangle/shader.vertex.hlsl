struct VSInput
{
	float3 a_position : TEXCOORD0;
	float4 a_color : TEXCOORD1;
};

struct VSOutput
{
	float4 v_color : TEXCOORD0;
	float4 position : SV_Position;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	output.position = float4(input.a_position, 1.0f);
	output.v_color = input.a_color;
	return output;
}