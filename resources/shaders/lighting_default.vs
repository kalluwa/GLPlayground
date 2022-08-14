#version 330

//position [0,1]
in vec2 position;

uniform vec4 rect;


out vec2 vs_uv;
void main()
{
	vs_uv = vec2(position.x,1.0-position.y);

	vec2 offset_pos = position * rect.zw + rect.xy;
	gl_Position = vec4(offset_pos,0,1);
}