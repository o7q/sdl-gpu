struct Input
{
	uint vertID : SV_VERTEXID;
};

struct Output
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};

Output main(Input input)
{
	Output output;

	const float2 verts[6] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1) };

	float2 vert = verts[input.vertID];
	
	output.position = float4(vert, 0.0f, 1.0f);
	output.uv = (vert + 1.0f) * 0.5f;
	return output;
}
