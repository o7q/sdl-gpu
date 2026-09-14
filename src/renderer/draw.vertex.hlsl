struct Input
{
	uint instanceID : SV_INSTANCEID;
	uint vertID : SV_VERTEXID;
};

struct Output
{
	float4 position : SV_POSITION;
	int objectType : TEXCOORD0;
	float2 uv : TEXCOORD1;
	float2 uvPos : TEXCOORD2;
	float2 uvSize : TEXCOORD3;
};

struct TextureObject
{
	float2 uvPos;
	float2 uvSize;
    float2 position;
    float2 size;
    float angle;
    float _pad[3];
};

struct RectangleObject
{
    float2 position;
    float2 size;
    float angle;
    float _pad[3];
};

struct EllipseObject
{
    float2 position;
    float2 size;
	float angle;
    float _pad[3];
};

struct DrawObject
{
    int objectType;
    int index;
    int2 _pad;
};

StructuredBuffer<DrawObject> objectBuffer : register(t0, space0);
StructuredBuffer<TextureObject> textureBuffer : register(t1, space0);
StructuredBuffer<RectangleObject> rectangleBuffer : register(t2, space0);
StructuredBuffer<EllipseObject> ellipseBuffer : register(t3, space0);

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

float2 handleStandardTransform(float2 position, float2 size, float angle, float2 vert)
{
	vert *= size / screenSize;
	vert -= 1.0f;

	vert = normalToScreen(vert);

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
	DrawObject object = objectBuffer[input.instanceID];

	switch (object.objectType)
	{
		case 0:
		{
			TextureObject tex = textureBuffer[object.index];
			vert = handleStandardTransform(tex.position, tex.size, tex.angle, vert);
			output.uvPos = tex.uvPos;
			output.uvSize = tex.uvSize;
		}
		break;
		case 1:
		{
			RectangleObject rect = rectangleBuffer[object.index];
			vert = handleStandardTransform(rect.position, rect.size, rect.angle, vert);
		}
		break;
		case 2:
		{
			EllipseObject ellipse = ellipseBuffer[object.index];
			vert = handleStandardTransform(ellipse.position, ellipse.size, ellipse.angle, vert);
		}
		break;
	}

	output.position = float4(vert, 0.0f, 1.0f);
	output.objectType = object.objectType;
	output.uv = uv;

	return output;
}