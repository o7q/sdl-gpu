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
    // 0: rectangle
    // 1: ellipse
    // used to determine which object buffer to read from
    int objectType;
    // index into the object buffer
    int index;
    int2 _pad;
};

StructuredBuffer<DrawObject> objectBuffer : register(t0, space0);
StructuredBuffer<RectangleObject> rectangleBuffer : register(t1, space0);
StructuredBuffer<EllipseObject> ellipseBuffer : register(t2, space0);

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

float2 handleRectangle(RectangleObject rectangle, float2 vert)
{
	vert *= rectangle.size / screenSize;
	vert -= 1.0f;

	vert = normalToScreen(vert);

	float newX = vert.x * cos(rectangle.angle) - vert.y * sin(rectangle.angle);
	float newY = vert.x * sin(rectangle.angle) + vert.y * cos(rectangle.angle);
	vert = float2(newX, newY);

	vert += rectangle.position;

	vert = screenToNormal(vert);

	return vert;
}

float2 handleEllipse(EllipseObject ellipse, float2 vert)
{
	vert *= ellipse.size / screenSize;
	vert -= 1.0f;

	vert = normalToScreen(vert);

	float newX = vert.x * cos(ellipse.angle) - vert.y * sin(ellipse.angle);
	float newY = vert.x * sin(ellipse.angle) + vert.y * cos(ellipse.angle);
	vert = float2(newX, newY);

	vert += ellipse.position;

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
			RectangleObject rect = rectangleBuffer[object.index];
			vert = handleRectangle(rect, vert);
		}
		break;
		case 1:
		{
			EllipseObject ellipse = ellipseBuffer[object.index];
			vert = handleEllipse(ellipse, vert);
		}
		break;
	}

	output.position = float4(vert, 0.0f, 1.0f);
	output.objectType = object.objectType;
	output.uv = uv;

	return output;
}