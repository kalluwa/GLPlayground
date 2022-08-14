#version 330

in vec3 worldpos;
in vec3 worldnormal;
in vec2 vs_uv;
in float vs_depth;

uniform vec3 cam_pos;
uniform vec2 viewport_size;
uniform sampler2D tex_diffuse;

uniform bool use_tex;
uniform vec3 diffuse_color;

layout(location = 0) out vec3 out_world_pos;
layout(location = 1) out vec4 out_world_normal;
layout(location = 2) out vec3 out_diffuse_color;

void main()
{
    
    out_world_pos = worldpos;
    out_world_normal.xyz = worldnormal;
    out_world_normal.w = vs_depth;

    if(use_tex)
        out_diffuse_color = texture(tex_diffuse,vs_uv).xyz;//vec3(vs_uv,1.0);//
    else
        out_diffuse_color = diffuse_color;
}
