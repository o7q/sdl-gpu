struct Input
{
	uint vertID : SV_VERTEXID;
};

struct Output
{
	float2 texCoord : TEXCOORD0;
	float4 Position : SV_Position;
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
	
	float2 texCoord = vert;

	if (screenRatio > drawRatio)
	{
		vert.x = (vert.x * drawRatio) / screenRatio;
	}
	else
	{
		vert.y = (vert.y / drawRatio) * screenRatio;
	}

	
	// if (screenRatio > drawRatio)
	// {
	// 	vert.x /= screenRatio;
	// }
	// else {
	// 	// vert.x *= screenRatio;
	// }
	// vert /= 2.0f;

	output.Position = float4(vert, 0.0f, 1.0f);
	output.texCoord = (texCoord + 1.0f) * 0.5f;
	return output;
}
