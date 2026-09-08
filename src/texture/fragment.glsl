#version 450

layout(set = 2, binding = 0) uniform sampler2D Sampler;

layout(location = 0) in vec2 TexCoord;
layout(location = 0) out vec4 FragColor;

void main()
{
    vec2 coord = TexCoord;
    FragColor = texture(Sampler, TexCoord);
}