struct Input
{
	int objectType : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

Texture2D<float4> swapchainTextue : register(t0, space2);
SamplerState swapchainSampler : register(s0, space2);

float4 main(Input input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    switch (input.objectType)
    {
        case 0:
        {
            color = float4(input.uv, 1.0f, 1.0f);
        }
        break;
        case 1:
        {
            float2 uv = input.uv - 0.5f;

            if (uv.x * uv.x + uv.y * uv.y < 0.25f)
            {
                color = float4(input.uv, 1.0f, 1.0f);
            }
        }
        break;
    }

    return color;
}
