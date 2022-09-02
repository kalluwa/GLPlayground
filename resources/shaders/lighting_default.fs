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

vec4 scPostW;
// From http://fabiensanglard.net/shadowmappingVSM/index.php
float chebyshevUpperBound(float distance)
{
	vec2 moments = texture2D(shadowMap,scPostW.xy).rg;
	
	// Surface is fully lit. as the current fragment is before the light occluder
	if (distance <= moments.x)
		return 1.0;

	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float variance = moments.y - (moments.x*moments.x);
	//variance = max(variance, 0.000002);
	variance = max(variance, 0.00002);

	float d = distance - moments.x;
	float p_max = variance / (variance + d*d);

	return p_max;
}

void main()
{
    //color = texture(shadowMap,vs_uv).xyz;
    //return;

    color = texture(diffuse_map,vs_uv).xyz;

    vec3 col = vec3(1.0,0.9,0.7);
    
    vec3 worldpos = texture(position_map,vs_uv).xyz;
    vec3 worldnormal = texture(normal_map,vs_uv).xyz;

    vec2 p = (2.0*gl_FragCoord.xy-viewport_size.xy)/viewport_size.y;

    //shadow
    vec4 sc = cameraToShadowProjector * vec4(worldpos,1.0);
	scPostW = sc / sc.w; 
	scPostW = scPostW * 0.5 + 0.5;

    //color = vec3(scPostW.xy,0);
    //color = texture(shadowMap,scPostW.xy).xyz;
    //return;
	float shadowFactor = 1.0; // Not in shadow
    bool outsideShadowMap = sc.w <= 0.0f || (scPostW.x < 0 || scPostW.y < 0) || (scPostW.x >= 1 || scPostW.y >= 1);
	if (!outsideShadowMap) 
	{
		shadowFactor = chebyshevUpperBound(scPostW.z);
	}

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
    color = color * col;// * shadowFactor;

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