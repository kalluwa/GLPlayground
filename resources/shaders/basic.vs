#version 330

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 view;
uniform mat4 proj;

out vec4 color;
out vec3 worldpos;
out vec3 worldnormal;

void main()
{
	color = vec4(abs(normal), 1.0);
	worldnormal = normalize(normal);
	worldpos = (view * vec4(position, 1)).xyz;
	gl_Position = proj * view * vec4(position, 1);
}