#version 330

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 worldpos;
out vec3 worldnormal;
out vec2 vs_uv;

out float vs_depth;

void main()
{
	vs_uv = uv;
	worldnormal = normalize(normal);
	vec4 view_pos = (view * model * vec4(position, 1));
	worldpos = (model * vec4(position,1.0)).xyz;

	gl_Position = proj * view * model * vec4(position, 1);

	
	vs_depth = gl_Position.z;// / gl_Position.w;
}