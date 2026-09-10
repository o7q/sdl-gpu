struct PSInput
{
    float4 v_color : TEXCOORD0;
};

float4 main(PSInput input) : SV_Target0
{
    return input.v_color;
}