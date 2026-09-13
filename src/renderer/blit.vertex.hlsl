struct Input
{
	uint vertID : SV_VERTEXID;
};

struct Output
{
	float4 position : SV_Position;
	float2 uv : TEXCOORD0;
};

cbuffer Uniforms : register(b0, space1)
{
	float screenRatio;
	float drawRatio;
};

Output main(Input input)
{
	Output output;

	const float2 verts[6] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1) };

	float2 vert = verts[input.vertID];
	
	float2 uv = vert;

	if (screenRatio > drawRatio)
	{
		vert.x = (vert.x * drawRatio) / screenRatio;
	}
	else
	{
		vert.y = (vert.y / drawRatio) * screenRatio;
	}

	output.position = float4(vert, 0.0f, 1.0f);
	output.uv = (uv + 1.0f) * 0.5f;
	return output;
}
