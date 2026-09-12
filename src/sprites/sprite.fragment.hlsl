struct Input
{
    float2 texPos : TEXCOORD0;
    uint instanceID : SV_INSTANCEID;
};

float4 main(Input input) : SV_TARGET
{
    return float4(input.texPos.xy, 0.0f, 0.5f);
}