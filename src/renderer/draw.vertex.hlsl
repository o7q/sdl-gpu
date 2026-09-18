struct TextureObject
{
	float2 atlasPositionNormal;
	float2 atlasSizeNormal;
	int atlasIndex;
	float angle;
	float2 position;
	float2 size;
	float2 origin;
	float2 cropTopLeft;
	float2 cropBottomRight;
};

struct GPUBasicShapeObject
{
	float2 position;
	float2 size;
	float2 origin;
	int fill;
	float outlineThickness;
	float4 color;
	float angle;
};

struct DrawObject
{
	int type;
	int index;
	float opacity;
	int filter;
};

struct Input
{
	uint instanceID : SV_INSTANCEID;
	uint vertID : SV_VERTEXID;
};

struct Output
{
	float4 position : SV_POSITION;
	uint objectIndex : TEXCOORD0;
	float2 uv : TEXCOORD1;
};

StructuredBuffer<DrawObject> objectBuffer : register(t0, space0);
StructuredBuffer<TextureObject> textureBuffer : register(t1, space0);
StructuredBuffer<GPUBasicShapeObject> shapeBuffer : register(t2, space0);

cbuffer Uniforms : register(b0, space1)
{
	float2 screenSize;
	uint objectStartIndex;
};

float2 normalToScreen(float2 normalPos)
{
	return (normalPos + 1.0f) * 0.5f * screenSize;
}

float2 screenToNormal(float2 screenPos)
{
	return screenPos / screenSize * 2.0f - 1.0f;
}

float2 handleStandardTransform(float2 position, float2 size, float2 origin, float angle, float2 vert)
{
	vert *= size / screenSize;
	vert -= 1.0f;

	vert = normalToScreen(vert);
	vert += size * 0.5f - origin;

	float newX = vert.x * cos(angle) - vert.y * sin(angle);
	float newY = vert.x * sin(angle) + vert.y * cos(angle);
	vert = float2(newX, newY);

	vert += position;

	vert = screenToNormal(vert);

	return vert;
}

Output main(Input input)
{
	Output output;

	const float2 verts[6] = { float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1) };

	float2 vert = verts[input.vertID];
	float2 uv = (vert + 1.0f) * 0.5f;
	DrawObject object = objectBuffer[objectStartIndex + input.instanceID];

	switch (object.type)
	{
		case 0:
		{
			TextureObject tex = textureBuffer[object.index];
			vert = handleStandardTransform(tex.position, tex.size, tex.origin, tex.angle, vert);
		}
		break;
		case 1:
		case 2:
		{
			GPUBasicShapeObject shape = shapeBuffer[object.index];
			vert = handleStandardTransform(shape.position, shape.size, shape.origin, shape.angle, vert);
		}
		break;
	}

	output.position = float4(vert, 0.0f, 1.0f);
	output.objectIndex = input.instanceID;
	output.uv = uv;

	return output;
}
