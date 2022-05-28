#version 330

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	vec4 world_pos = view * vec4(position,1.0);

	gl_Position = proj * world_pos;
}