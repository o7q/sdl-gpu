struct VSOutput
{
	float4 position : SV_POSITION;
	float2 pos : TEXCOORD0;
	float4 color : TEXCOORD1;
};

struct Sprite {
	float x, y, scale;
	float r, g, b, a;
};

StructuredBuffer<Sprite> sprites : register(t0, space0);

VSOutput main(uint id : SV_VERTEXID, uint instance : SV_INSTANCEID)
{
	VSOutput output;

	const float2 verts[6] = { float2(-2, 1), float2(1, 1), float2(-1, -1), float2(1, 1),float2(1, -1), float2(-1, -1) };

	Sprite sprite = sprites[instance];

	output.position = float4(verts[id] * sprite.scale + float2(sprite.x, sprite.y), 0.0f, 1.0f);

	output.pos = (output.position.xy + 1.0f) * 0.5f;
	output.color = float4(sprite.r, sprite.g, sprite.b, sprite.a);
	return output;
}