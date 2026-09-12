struct Input
{
	uint vertID : SV_VERTEXID;
	uint instanceID : SV_INSTANCEID;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 texPos : TEXCOORD0;
};

cbuffer Uniforms : register(b0, space1)
{
	int time;
	int2 screenSize;
};

float2 normalToScreen(float2 normalPos)
{
	return (normalPos + 1.0f) * 0.5f * screenSize;
}

float2 screenToNormal(float2 screenPos)
{
	return screenPos / screenSize * 2.0f - 1.0f;
}

Output main(Input input)
{
	Output output;

	const float2 verts[6] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1) };

	float2 vert = verts[input.vertID];

	vert = normalToScreen(vert);
	vert -= screenSize * 0.5;

	vert *= 0.5;

	float angle = time / 500.0f;


	float newX = vert.x * cos(angle) - vert.y * sin(angle);
	float newY = vert.x * sin(angle) + vert.y * cos(angle);

	vert = float2(newX, newY);
		vert += screenSize * 0.5;
	vert = screenToNormal(vert);

	output.position = float4(vert, 0.0f, 1.0f);
	output.texPos = (vert + 1.0f) * 0.5f;

	return output;
}