#version 450

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;

uniform mat4 view;
uniform mat4 proj;

layout(location=0) out vec4 color;
layout(location=1) out vec3 worldpos;
layout(location=2) out vec3 worldnormal;

void main()
{
	color = vec4(abs(normal), 1.0);
	worldnormal = normalize(normal);
	worldpos = (view * vec4(position, 1)).xyz;
	gl_Position = proj * view * vec4(position, 1);
}