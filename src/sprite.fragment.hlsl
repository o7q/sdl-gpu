struct Input
{
    float4 color : TEXCOORD0;
    float4 pixelPos : SV_POSITION;
};

float4 main(Input input) : SV_TARGET
{
    float4 color = input.color;

    if ((int)input.pixelPos.x % 2 == 0)
    {
        color *= 0.9f;
    }
    return color;
}