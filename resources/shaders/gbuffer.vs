#version 330

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 view;
uniform mat4 proj;

out vec3 worldpos;
out vec3 worldnormal;
out vec3 vs_uv;

void main()
{
	vs_uv = uv;
	worldnormal = normalize(normal);
	worldpos = (view * vec4(position, 1)).xyz;
	gl_Position = proj * view * vec4(position, 1);
}