struct VSInput
{
	float2 position : TEXCOORD0;
	uint instanceID : SV_INSTANCEID;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float2 nrmPos : TEXCOORD0;

};

cbuffer Uniforms : register(b0, space1)
{
	int2 screenSize;
	int time;
};

float2 toScreenPos(float2 normalizedPos)
{
	return (normalizedPos + 1.0f) * 0.5f * screenSize;
}

float2 toNormalPos(float2 screenPos)
{
	return (screenPos / screenSize) * 2.0f - 1.0f;
}

VSOutput main(VSInput input)
{
	VSOutput output;
	float2 outPos = input.position.xy;
	float triangleSize = 10;

	float screenMin = min(screenSize.x, screenSize.y);
	float scaleFactor = min(triangleSize, screenMin);

	outPos *= scaleFactor / screenSize;

	outPos = toScreenPos(outPos);

	int offset = input.instanceID;
    offset ^= offset >> 16;
    offset *= 0x85ebca6b;
    offset ^= offset >> 13;
    offset *= 0xc2b2ae35;
    offset ^= offset >> 16;
	offset = offset % 1000;

	outPos.x += cos(time/1000.0f + input.instanceID) * offset;
	outPos.y += sin(time/1000.0f + input.instanceID) * offset;

	outPos = toNormalPos(outPos);


	output.position = float4(outPos, 0.0f, 1.0f);
	output.nrmPos = (outPos + 1.0f) * 0.5f;
	return output;
}