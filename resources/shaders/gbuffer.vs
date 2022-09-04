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
out vec3 viewpos;

out float vs_depth;

void main()
{
	vs_uv = uv;
	mat3 normalMatrix = transpose(inverse(mat3(view * model)));
	worldnormal = normalMatrix * normal;
	vec4 view_pos = (view * model * vec4(position, 1));
	viewpos = view_pos.xyz;
	worldpos = (model * vec4(position,1.0)).xyz;

	gl_Position = proj * view * model * vec4(position, 1);

	
	vs_depth = gl_Position.z;// / gl_Position.w;
}