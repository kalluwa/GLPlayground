#version 460 core

#define sampleRadius 0.6f
#define depthBias  0.1f

in vec2 vs_uv;

uniform vec3 cam_pos;
uniform vec2 viewport_size;
uniform mat4 view;
uniform mat4 inv_view;
uniform mat4 proj;

uniform vec3 offset_positions[64];

out vec3 color;

//shadowmap
uniform mat4 cameraToShadowProjector;
uniform sampler2DArray shadowMap;

uniform sampler2D position_map;
uniform sampler2D normal_map;
uniform sampler2D diffuse_map;
uniform sampler2D noise_map;

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform float farPlane;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform int cascadeCount;   // number of frusta - 1

float ShadowCalculation(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = texture(normal_map,vs_uv).xyz;//normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    const float biasModifier = 0.5f;
    if (layer == cascadeCount)
    {
        bias *= 1 / (farPlane * biasModifier);
    }
    else
    {
        bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
        
    return shadow;
}

vec3 getDiffuse()
{
    vec3 diffuse = texture(diffuse_map,vs_uv).xyz;
    
    vec3 col = vec3(1.0,0.9,0.7);
        
    vec3 worldpos = (inv_view * vec4(texture(position_map,vs_uv).xyz,1.0)).xyz;
    vec3 worldnormal = texture(normal_map,vs_uv).xyz;
    
    vec2 p = (2.0*gl_FragCoord.xy-viewport_size.xy)/viewport_size.y;

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
    diffuse = diffuse * col;

    return diffuse;
}
vec3 testShadow()
{
    vec3 color = getDiffuse()*1.3f;//texture(diffuse_map,vs_uv).xyz;// texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(texture(normal_map,vs_uv).xyz);
    vec3 lightColor = vec3(0.9);
    // ambient
    vec3 ambient = 0.8 * color;
    // diffuse
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
     
    // specular
    vec3 fragPosView = texture(position_map,vs_uv).xyz;
    vec3 fragPos = (inv_view * vec4(fragPosView,1.0)).xyz;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fragPos);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    return lighting;
}
void main()
{
    color = testShadow();
    color = pow(color,vec3(1.0/2.2));
    return;
    //color = texture(shadowMap,vec3(vs_uv,0)).xyz;
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
    color = color * col;// * shadowFactor;

    //gamma fix
    //color = pow(color,vec3(1.0/2.2));

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