#version 330

//position [0,1]
in vec2 position;

uniform vec4 rect;

out vec2 vs_uv;
void main()
{
	vs_uv = position;

	vec2 offset_pos = vs_uv * rect.zw + rect.xy;
	gl_Position = vec4(position,0,1);
}