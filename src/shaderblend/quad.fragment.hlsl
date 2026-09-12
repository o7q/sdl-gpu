struct Input
{
	float2 position : TEXCOORD0;
    float4 color : TEXCOORD1;
};

float4 main(Input input) : SV_TARGET
{
    return input.color;
}