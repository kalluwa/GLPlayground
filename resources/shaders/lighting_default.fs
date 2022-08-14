#version 330

#define sampleRadius 0.6f
#define depthBias  0.1f

in vec2 vs_uv;

uniform vec3 cam_pos;
uniform vec2 viewport_size;
uniform mat4 view;
uniform mat4 inv_view;
uniform mat4 proj;

//shadowmap
uniform mat4 cameraToShadowProjector;
uniform sampler2D shadowMap;

uniform sampler2D position_map;
uniform sampler2D normal_map;
uniform sampler2D diffuse_map;
uniform sampler2D noise_map;

uniform vec3 offset_positions[64];

out vec3 color;

void main()
{
    //color = texture(shadowMap,vs_uv).xyz;
    //return;

    color = texture(diffuse_map,vs_uv).xyz;

    vec3 col = vec3(1.0,0.9,0.7);
    
    vec3 worldpos = texture(position_map,vs_uv).xyz;
    vec3 worldnormal = texture(normal_map,vs_uv).xyz;

    vec2 p = (2.0*gl_FragCoord.xy-viewport_size.xy)/viewport_size.y;

    //vec3 col = render( p );
    {
        vec3 rd = -normalize(cam_pos - worldpos);
        vec3 pos = worldpos;
        vec3 nor = normalize(worldnormal);
        vec3 ref = reflect( rd, nor );
        float fre= clamp( 1.0 + dot(nor,rd), 0.0, 1.0 );
        	
        float occ = 0.45;
        	
        col = vec3(0.9,0.02,0.01);
        col = col*0.72 + 0.2*fre*vec3(1.0,0.8,0.2);
        float max_n = max(nor.y,nor.y);
        max_n = max(nor.y,max_n);
        vec3 lin  = 4.0*vec3(0.7,0.80,1.00)*(0.5+0.5*max_n)*pow(occ,0.5);
                lin += 0.8*fre*vec3(1.0,1.0,1.00)*(0.6+0.4*occ);
        col = col * lin;
        col += 2.5*vec3(0.8,0.9,1.00)*smoothstep(0.0,0.4,ref.y)*(0.06+0.94*pow(fre,5.0))*occ;
    		
    	col = pow(col,vec3(0.4545));
    }
    col = clamp(col,0.0,1.0);
        
    vec2 q = gl_FragCoord.xy/viewport_size.xy;
    col *= 0.2 + 0.8*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.2);

    vec3 ambient = vec3(0.2f,0.2f,0.2f);
    color = color * col;

    //gamma fix
    color = pow(color,vec3(1.0/2.2));

 //   //ao calculation
 //   vec3 view_pos = (view * vec4(worldpos,1.0)).xyz;

 //   vec3 view_normal = mat3(view) *  worldnormal;
 //   view_normal = normalize(view_normal);
 //   vec3 rand_vec = 2.0f*texture(noise_map,vs_uv*viewport_size.xy*0.25).xyz-1.0f;
 //   vec3 tangent = normalize(rand_vec - view_normal* dot(rand_vec,view_normal));
 //   vec3 bitangent = normalize(cross(view_normal,tangent));
 //   mat3 tbn_mat = transpose(mat3(tangent,bitangent,view_normal));

	//float oc = 0.0;
 //   vec2 uv_debug = vec2(0,0);
 //   float depth_check = texture(normal_map,vs_uv).a;
 //   const int count = 16;
	//for (int i = 0; i < count; i++)
	//{
	//	vec3 offset = tbn_mat * offset_positions[i];//normalize(tbn_mat * normalize(offset_positions[i]));//tbn_mat * 

	//	//float flip = sign(dot(offset, viewNormal));
	//	vec4 randPos = view * vec4(worldpos + sampleRadius * offset,1.0);
	//	vec4 ndcPos = proj * randPos;
	//	ndcPos /= ndcPos.w;
	//	vec2 screenPos = ndcPos.xy*0.5f+0.5f;
	//	//screenPos.y = 1.0f - screenPos.y;

	//	float randDepth = texture(normal_map,screenPos).a;

	//	float range = smoothstep(0.0f, 1.0f, sampleRadius / abs(randDepth - depth_check));
	//	float ao = (randDepth + depthBias < depth_check) ? 1.0f : 0.0f;
	//	oc += ao * range;

 //       uv_debug += abs(screenPos - vs_uv);
	//}
	//oc = 1.0f - oc / count;

 //   uv_debug /= count;

 //   color *= pow(oc,1/2.2);
}