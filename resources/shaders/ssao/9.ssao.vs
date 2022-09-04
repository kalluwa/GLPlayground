#version 330 core
layout(location = 0) in vec2 position;

uniform vec4 rect;

out vec2 TexCoords;

void main()
{
    TexCoords = vec2(position.x,1.0-position.y);
    vec2 offset_pos = position * rect.zw + rect.xy;
    gl_Position = vec4(offset_pos,0,1.0);
}