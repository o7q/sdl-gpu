struct Input
{
	float2 position : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET
{
    return float4(input.position.xy, 1.0f, 1.0f);
}