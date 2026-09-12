struct Input
{
	uint vertID : SV_VERTEXID;
	uint instanceID : SV_INSTANCEID;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct Sprite
{
	float2 position;
	float2 size;
	float angle;
	float3 _pad;
	float4 color;
};	

StructuredBuffer<Sprite> spriteBuffer : register(t0);

cbuffer Uniforms : register(b0, space1)
{
	float2 screenSize;
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
	float2 uv = (vert + 1.0f) * 0.5f;
	Sprite sprite = spriteBuffer[input.instanceID];

	float newX = vert.x * cos(sprite.angle) - vert.y * sin(sprite.angle);
	float newY = vert.x * sin(sprite.angle) + vert.y * cos(sprite.angle);
	vert = float2(newX, newY);

	vert *= sprite.size / screenSize;
	vert -= 1.0f;

	vert = normalToScreen(vert);

	vert += sprite.position;

	vert = screenToNormal(vert);

	output.position = float4(vert, 0.0f, 1.0f);
	output.uv = uv;

	return output;
}