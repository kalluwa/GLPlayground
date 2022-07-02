#version 330

in vec3 worldpos;
in vec3 worldnormal;
in vec2 vs_uv;

uniform vec3 cam_pos;
uniform vec2 viewport_size;
uniform sampler2D tex_diffuse;

layout(location = 0) out vec3 out_world_pos;
layout(location = 1) out vec3 out_world_normal;
layout(location = 2) out vec3 out_diffuse_color;

void main()
{
    
    out_world_pos = worldpos;
    out_world_normal = worldnormal;
    
    out_diffuse_color = texture(tex_diffuse,vs_uv).xyz;//vec3(vs_uv,1.0);//
}
