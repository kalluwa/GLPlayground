#version 330

in vec2 vs_uv;

uniform sampler2D diffuse;

out vec3 color;

void main()
{
    color = texture(diffuse,vs_uv).xyz;
}